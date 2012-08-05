/**<!-------------------------------------------------------------------->
   @file   ThinLensCamera.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2008
   
   @brief
      Thin lens camera approximation supporting common camera inputs and 
   allowing for depth of field
   
   @param
      aperture - diameter of the aperature of the lens in mm (eg. 35mm, 50mm)
   @param
      fstop    - (aka F number), is defined as the unitless ratio of the focal 
                 length to the diameter of the aperture of the lens
   @param
      focalDistance - the distance in meters between the lens and the focal 
                      plane (objects lying on the focal plane will be in 
                      perfect focus)
   @param
      focalPoint    - (optional) user may specify a focalPoint in NDC on the 
                      film-plane instead of specifying a focalDistance to 
                      "auto-focus" the object intersected by a ray traced 
                      through the corresponding pixel, whose distance away 
                      from the lens will be used as the focalDistance.
                      @note focalPoint is expressed as a vector2 in the unit 
                      square [0,1]^2, where [0,0] is the lower left corner 
                      of the image, and [1,1] is the upper-right corner
   
   (focal length is derived from fstop and aperture)
   
   @see http://www.eecs.berkeley.edu/~barsky/VisRendPapers/survey1.pdf
   <!-------------------------------------------------------------------->**/

#include "ThinLensCamera.h"
#include <ResourceManager.h>
#include <Material.h>
#include <Viewport.h>
#include <Scene.h>
#include <algebra.h>
#include <QtCore/QtCore>

namespace milton {

ThinLensCamera::ThinLensCamera() 
   : Camera()
{
   m_aspect = 1.0;
   m_hAngle = 45.0;
   m_near   = .05;
   m_far    = 50.0;
   
   Point3 eye(2, 2, 2);
   Point3 origin;
   
   // TODO: dangerous calling virtual method in constructor...
   setOrientation(eye, origin - eye, Vector3(0, 1, 0));
}

ThinLensCamera::~ThinLensCamera()
{ }

void ThinLensCamera::init(Scene *scene) {
   { // initialize normal camera orientation / position parameters
      const Point3 &eye   = getValue<Point3> ("eye",  m_eye);
      const Point3 &focus = getValue<Point3> ("focus", Point3());
      const Vector3 &look = getValue<Vector3>("look", (focus - eye).getNormalized());
      Vector3 up          = getValue<Vector3>("up",   m_v);
      
      if (EQ(ABS(look.dot(up)), 1.0)) {
         dprintf("Error: look vector collinear to up vector; switching to default up\n");
         
         if (up != Vector3(0, 1, 0))
            up = Vector3(0, 1, 0);
         else up = Vector3(0, 0, 1);
      }
      
      m_aspect = getValue<real_t>("aspectRatio", m_aspect);
      m_hAngle = getValue<real_t>("heightAngle", m_hAngle);
      
      // near and far clipping planes are for OpenGL preview only
      m_near   = getValue<real_t>("near", m_near);
      m_far    = getValue<real_t>("far",  m_far);
      
      // initialize camera's internal orthonormal basis
      setOrientation(eye, look, up);
   }
   
   { // initialize thin lens parameters
      // parse thin lens inputs (defaults to a 35mm camera whose focal length is 
      // .49m); convert aperture to meters for all internal computations
      m_aperture    = getValue<real_t>("aperture", create_real(35.0)) / 1000.0;
      m_fstop       = getValue<real_t>("fstop",    create_real(14));
      
      // fstop is defined as the ratio of focalLength to aperture diameter
      m_focalLength = m_fstop * m_aperture;
      bool autoFocus = false;
      
      if (contains("focalPoint") || !contains("focalDistance")) {
         static real_t defaultTarget[] = { create_real(0.5), create_real(0.5) };
         
         // automatically calculate focal distance ("autofocus")
         const real_t *target = getValue<real_t*>("focalPoint", defaultTarget);
         const Point3 fp(CLAMP(target[0], 0, 1) - 0.5, .5 - CLAMP(target[1], 0, 1), -1);
         
         const Vector3 &direction = (m_filmToWorld * fp - m_eye).getNormalized();
         Ray ray(m_eye, direction);
         SurfacePoint pt;
         
         ASSERT(scene);
         if (scene->isInitialized()) {
            m_focalDistance = scene->getIntersection(ray, pt);
            
            autoFocus = Ray::isValid(m_focalDistance);
         }
      }
      
      if (!autoFocus)
         m_focalDistance = getValue<real_t>("focalDistance", 1);
      
      ResourceManager::log << "ThinLensCamera: aperture      = " << 
         m_aperture * 1000.0 << " mm" << endl;
      ResourceManager::log << "ThinLensCamera: fstop         = " << 
         m_fstop << endl;
      ResourceManager::log << "ThinLensCamera: focalLength   = " << 
         m_focalLength << " m" << endl;
      ResourceManager::log << "ThinLensCamera: focalDistance = " << 
         m_focalDistance << " m" << (autoFocus ? " (autofocused)" : "") << endl;
   }
}

Ray ThinLensCamera::getWorldRay(const Point2 &pt) const {
   const Point3 ndc(2 * pt[0] - 1, 1 - 2 * pt[1], -1);
   const Vector3 &direction = (m_filmToWorld * ndc - m_eye).getNormalized();
   
   Ray ray(m_eye, direction);
   
   // compute circle of confusion and shift ray proportionately if this camera 
   // has a non-zero aperture (if it's a pinhole, the aperture is infinitesmal 
   // s.t. all rays are in focus and the ray's origin and direction are 
   // deterministic)
   if (m_aperture > 0) {
      // compute intersection with focal plane at distance m_focalDistance
      const real_t t = -m_focalDistance / (direction.dot(m_n));
      ASSERT(t >= -EPSILON);
      
      if (t == m_focalLength)
         return ray;
      
      // calculate new focal point for this ray where it would intersect the 
      // look vector after being "refracted"
      const Point3 &focus = ray.origin + direction * t;
      
      // distance between lens and actual film plane
      const real_t dImage = m_focalDistance;
      
      // distance between lens and perfect film plane on image side for this ray
      const real_t lImage = m_focalLength * t / 
         (t - m_focalLength);
      
      // radius of circle of confusion on image plane
      const real_t cofRadius = m_aperture * (lImage - dImage) / lImage;
      
      // sample uniformly within disc defined by circle of confusion
      const real_t e1 = Random::sample(0, M_PI / 2);
      const real_t e2 = Random::sample(0, 2 * M_PI);
      const real_t r  = cofRadius * sin(e1);
      
      // shift ray origin and compute new ray direction
      ray.origin += r * cos(e2) * m_u + r * sin(e2) * m_v;
      ray.direction = (focus - ray.origin).getNormalized();
      ASSERT(ray.direction.isUnit());
   }
   
   return ray;
}

void ThinLensCamera::fillGLMatrix(real_t *buffer) {
   m_worldToFilm.fillGLMatrix(buffer);
}

void ThinLensCamera::_computeMatrix() {
   ASSERT(m_far > 0 && m_near < m_far);
   const real_t farInv = 1.0 / m_far;
   const real_t k = m_near * farInv;
   
   const Matrix4x4 A(1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1 / (k - 1), k / (k - 1),
                     0, 0, -1, 0);
   
   const Matrix4x4 B(farInv, 0, 0, 0,
                     0, farInv, 0, 0,
                     0, 0, farInv, 0,
                     0, 0, 0, 1);
   
   const real_t vert_angle = m_hAngle * (M_PI / 180);
   
   real_t cot_h, cot_w, tan_h, tan_w;
   cot_h = 1.0f / tan(vert_angle / 2);
   cot_w = cot_h / m_aspect;
   
   tan_h = tan(vert_angle / 2);
   tan_w = m_aspect * tan_h;
   
   const Matrix4x4 C(cot_w, 0, 0, 0,
                     0, cot_h, 0, 0,
                     0, 0,     1, 0,
                     0, 0,     0, 1);

   const Matrix4x4 D(m_u[0], m_u[1], m_u[2], 0,
                     m_v[0], m_v[1], m_v[2], 0,
                     m_n[0], m_n[1], m_n[2], 0,
                     0, 0, 0, 1);

   const Matrix4x4 E(1, 0, 0, -(m_eye[0]),
                     0, 1, 0, -(m_eye[1]),
                     0, 0, 1, -(m_eye[2]),
                     0, 0, 0, 1);
   
   m_worldToFilm = (A * B * C * D * E);
   
   m_filmToWorld =
      Matrix4x4(1, 0, 0, m_eye[0],
                0, 1, 0, m_eye[1],
                0, 0, 1, m_eye[2],
                0, 0, 0, 1) *
      Matrix4x4(m_u[0], m_v[0], m_n[0], 0,
                m_u[1], m_v[1], m_n[1], 0,
                m_u[2], m_v[2], m_n[2], 0,
                0, 0, 0, 1) *
      Matrix4x4(tan_w, 0, 0, 0,
                0, tan_h, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1) *
      Matrix4x4(m_far, 0, 0, 0,
                0, m_far, 0, 0,
                0, 0, m_far, 0,
                0, 0, 0, 1);
}

void ThinLensCamera::setOrientation(const Point3 &p, const Vector3 &l, 
                                    const Vector3 &up)
{
   m_eye = p;
   m_n = -l;
   m_u = up.cross(m_n);
   m_n.normalize();
   m_u.normalize();
   m_v = m_n.cross(m_u);
   
   ASSERT(m_u.isUnit());
   ASSERT(m_v.isUnit());
   ASSERT(m_n.isUnit());
   ASSERT(EQ(m_n.dot(m_u), create_real(0)));
   ASSERT(EQ(m_n.dot(m_v), create_real(0)));
   ASSERT(EQ(m_u.dot(m_v), create_real(0)));
   
   _computeMatrix();
}

real_t ThinLensCamera::getIntersection(const Ray &ray, SurfacePoint &pt) {
   return INFINITY;
}
 
Point2 ThinLensCamera::getProjection(const Point3 &p) const {
   Point3 p1 = m_worldToFilm * p;
   
   homogenize(p1);
   return Point2((p1[0] + 1) * .5, (1 - p1[1]) * .5);
}

}

