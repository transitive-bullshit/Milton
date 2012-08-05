/**<!-------------------------------------------------------------------->
   @file   Plane.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a unit plane in the y=0 plane from (-.5,0,-.5) 
   to (.5,0,.5)
   <!-------------------------------------------------------------------->**/

#include "Plane.h"
#include "SurfacePoint.h"
#include <Random.h>

#include <GL/gl.h>

namespace milton {

void Plane::init() {
   m_objSpaceAABB = AABB(Vector3(-.5, 0, -.5), Vector3(.5, 0, .5));
   
   Transformable::init();
}

real_t Plane::getIntersection(const Ray &ray, SurfacePoint &pt) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   const real_t t = -P[1] / D[1];
   
   if (t > INTERSECT_TOLERANCE && 
       fabs(P[0] + t * D[0]) <= 0.5 && 
       fabs(P[2] + t * D[2]) <= 0.5)
      return t;
   
   return INFINITY;
}

bool Plane::intersects(const Ray &ray, real_t tMax) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   const real_t t = -P[1] / D[1];
   
   return (isValidT(t, tMax) && 
           fabs(P[0] + t * D[0]) <= 0.5 && 
           fabs(P[2] + t * D[2]) <= 0.5);
}

void Plane::_getUV(SurfacePoint &pt) const {
   Point3 P;
   
   _transformPoint3WorldToObj(pt.position, P);
   
   pt.uv.u = CLAMP(P[0] + 0.5, 0, 1);
   pt.uv.v = CLAMP(P[2] + 0.5, 0, 1);
}

void Plane::_getGeometricNormal(SurfacePoint &pt) const {
   Vector3 nObj(0, 1, 0);
   
   _transformVector3ObjToWorld(nObj, pt.normalG);
}

void Plane::preview() {
   Transformable::preview();
   
   glBegin(GL_QUADS);
   
   glNormal3f(0, 1, 0);
   
   glVertex3f(-.5, 0, -.5);
   glVertex3f(-.5, 0,  .5);
   glVertex3f( .5, 0,  .5);
   glVertex3f( .5, 0, -.5);
   
   glEnd();
}

#if 0
Vector3 Plane::getNormal(const Point3 &pt) const {
   Vector3 nObj(0, 1, 0), nWorld;
   
   _transformVector3ObjToWorld(nObj, nWorld);
   return nWorld;
}
#endif

Point3 Plane::getPosition(const UV &uv) {
   return m_transToWorld * Point3(-.5 + uv.u, 0, -.5 + uv.v);
}

real_t Plane::_getSurfaceArea() {
   const Point3 &p1 = m_transToWorld * Point3(-.5, 0, -.5);
   const Point3 &p2 = m_transToWorld * Point3( .5, 0, -.5);
   //const Point3 &p3 = m_transToWorld * Point3( .5, 0,  .5);
   const Point3 &p4 = m_transToWorld * Point3(-.5, 0,  .5);
   
   const Vector3 &e1 = p2 - p1;
   const Vector3 &e2 = p4 - p1;
   
   return e1.cross(e2).getMagnitude();
}

}

