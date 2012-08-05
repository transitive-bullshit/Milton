/**<!-------------------------------------------------------------------->
   @file   Sphere.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a 3D sphere with radius .5, enclosed in the unit box 
   (-.5,-.5,-.5) to (.5,.5,.5)
   <!-------------------------------------------------------------------->**/

#include "Sphere.h"
#include <SurfacePoint.h>

namespace milton {

real_t Sphere::getIntersection(const Ray &ray, SurfacePoint &pt) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   // A = (Dx^2 + Dy^2 + Dz^2)
   const real_t A = 2 * (D[0] * D[0] + D[1] * D[1] + D[2] * D[2]);
   
   // B = (2 * Px * Dx + 2 * Py * Dy + 2 * Pz * Dz)
   const real_t B = -2 * (P[0] * D[0] + P[1] * D[1] + P[2] * D[2]);
   
   // C = (Px^2 + Py^s + Pz^2 - 1/4)
   const real_t C = (P[0] * P[0] + P[1] * P[1] + P[2] * P[2] - 0.25);
   
   // Discriminant of quadratic = B^2 - 4 * A * C
   real_t discriminant =  B * B - 2 * A * C;
   
   // Check for intersection with Sphere Body
   // ---------------------------------------
   if (discriminant < 0)
      return INFINITY;
   
   discriminant = sqrt(discriminant);
   const real_t F = 1 / A;
   real_t       t = (B - discriminant) * F;
   
   return ((t > INTERSECT_TOLERANCE || 
           (t = (B + discriminant) * F) > INTERSECT_TOLERANCE) 
          ? t : INFINITY);
}

bool Sphere::intersects(const Ray &ray, real_t tMax) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   // A = (Dx^2 + Dy^2 + Dz^2)
   const real_t A = 2 * (D[0] * D[0] + D[1] * D[1] + D[2] * D[2]);
   
   // B = (2 * Px * Dx + 2 * Py * Dy + 2 * Pz * Dz)
   const real_t B = -2 * (P[0] * D[0] + P[1] * D[1] + P[2] * D[2]);
   
   // C = (Px^2 + Py^s + Pz^2 - 1/4)
   const real_t C = (P[0] * P[0] + P[1] * P[1] + P[2] * P[2] - 0.25);
   
   // Discriminant of quadratic = B^2 - 4 * A * C
   real_t discriminant =  B * B - 2 * A * C;
   
   // Check for intersection with Sphere Body
   // ---------------------------------------
   
   if (discriminant >= 0) {
      discriminant = sqrt(discriminant);
      const real_t F = 1 / A;
      
      real_t t = (B + discriminant) * F;
      return (isValidT(t, tMax) || 
              ((t = (B - discriminant) * F) < tMax && t > INTERSECT_TOLERANCE));
   }
   
   return false;
}

void Sphere::_getUV(SurfacePoint &pt) const {
   Point3 P;
   _transformPoint3WorldToObj(pt.position, P);
   real_t &u = pt.uv.u, &v = pt.uv.v;
   
   u = 1 - atan2(P[2], P[0]) * (1 / (2 * M_PI));
   v = 0.5 - asin(P[1] * 2) * (1 / M_PI);
}

void Sphere::_getGeometricNormal(SurfacePoint &pt) const {
   Point3 P;
   _transformPoint3WorldToObj(pt.position, P);
   Vector3 nObj(P[0], P[1], P[2]);
   
   _transformVector3ObjToWorld(nObj, pt.normalG);
}

void Sphere::preview() {
   Transformable::preview();
   
   if (m_quadric == NULL)
      m_quadric = gluNewQuadric();
   
   ASSERT(m_quadric != NULL);
   
   gluSphere(m_quadric, 0.5, 20, 20);
}

void Sphere::getRandomPoint(SurfacePoint &pt) {
   if (!m_surfaceSampler)
      m_surfaceSampler = new UniformOnSphereSampler();
   
   pt.position = m_transToWorld * m_surfaceSampler->sample().getValue<Point3>();
   _getUV(pt);
   
   getPoint(pt, pt.uv);
}

Point3 Sphere::getPosition(const UV &uv) {
   real_t x = CLAMP(tan((1.0 - uv.u) / (2 * M_PI)), -.5, .5);
   real_t y = CLAMP(sin((0.5 - uv.v) / M_PI) * 0.5, -.5, .5);
   real_t z = sqrt(1 - x * x - y * y);
   
   // TODO: verify correctness for x and z (inversing atan2)
   return m_transToWorld * Point3(x, y, z);
}

#if 0
Vector3 Sphere::getNormal(const Point3 &pt) const {
   const Point3 &ptObj = m_transToWorldInv * pt;
   Vector3 nObj(ptObj.data), nWorld;
   
   // TODO: shouldn't need to normalize nObj before transformation
   _transformVector3ObjToWorld(nObj, nWorld);
   
   return nWorld;
}
#endif

real_t Sphere::_getSurfaceArea() {
   // TODO return surface area in world-space!!!
   return (4 * M_PI * 0.25); // 4*pi*r^2
}

}

