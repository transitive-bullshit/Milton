/**<!-------------------------------------------------------------------->
   @file   Cube.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a unit cube in 3-space from (-.5,-.5,-.5) to (.5,.5,.5)
   <!-------------------------------------------------------------------->**/

#include "Cube.h"
#include "SurfacePoint.h"
#include <Random.h>

#include <GL/glut.h>

namespace milton {

real_t Cube::getIntersection(const Ray &ray, SurfacePoint &pt) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   real_t minT = INFINITY;
   
   // Check for intersection with Sides of Cube
   // -----------------------------------------
   for(int u = 3, v = 0, w = 1; u--; w = v, v = u) {
      const real_t invD = 1 / D[u];
      const real_t t1   = (0.5 - P[u]) * invD;
      
      if (isValidT(t1, minT)) {
         const Point3 &p = P + t1 * D;
         
         if (fabs(p[v]) <= 0.5 && fabs(p[w]) <= 0.5) {
            pt.normalCase = u;
            minT = t1;
         }
      }
      
      const real_t t2 = (-0.5 - P[u]) * invD;
      if (isValidT(t2, minT)) {
         const Point3 &p = P + t2 * D;
         
         if (fabs(p[v]) <= 0.5 && fabs(p[w]) <= 0.5) {
            pt.normalCase = 4 + u;
            minT = t2;
         }
      }
   }
   
   return minT;
}

bool Cube::intersects(const Ray &ray, real_t tMax) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   // Check for intersection with Sides of Cube
   // -----------------------------------------
   for(int u = 3, v = 0, w = 1; u--; w = v, v = u) {
      const real_t invD = 1 / D[u];
      real_t t = (0.5 - P[u]) * invD;
      
      if (isValidT(t, tMax)) {
         const Point3 &p = P + t * D;
         
         if (p[v] > -0.5 && p[v] < 0.5 && p[w] > -0.5 && p[w] < 0.5)
            return true;
      }
      
      t = (-0.5 - P[u]) * invD;
      if (isValidT(t, tMax)) {
         const Point3 &p = P + t * D;
         
         if (p[v] > -0.5 && p[v] < 0.5 && p[w] > -0.5 && p[w] < 0.5)
            return true;
      }
   }
   
   return false;
}

void Cube::_getUV(SurfacePoint &pt) const {
   const Point3 &P = pt.position;
   real_t &u = pt.uv.u, &v = pt.uv.v;
   
   // 0 <= normalCase <= 6  (excluding 3)
   const char side = (pt.normalCase & 3);
   v = P[side + 1 - 3 * (side == 2)] + 0.5;
   u = P[side + 2 - 3 * (side >= 1)] + 0.5;
   
   if (side == 2) {
      const real_t c = v;
      v = u;
      u = 1 - c;
   }
   
   if (pt.normalCase & 4)
      v = 1 - v;
   
   if (!(pt.normalCase & 5)) {
      u = 1 - u;
      v = 1 - v;
   }
}

void Cube::_getGeometricNormal(SurfacePoint &pt) const {
   Vector3 nObj;
   nObj[pt.normalCase & 3] = 1 - ((pt.normalCase & 4) >> 1);
   
   _transformVector3ObjToWorld(nObj, pt.normalG);
}

void Cube::preview() {
   Transformable::preview();
   
   glutSolidCube(1.0);
}

real_t Cube::_getSurfaceArea() {
   // Transform e1, e2, e3 to world space and get
   // basis vectors b1, b2, b3
   const Point3 &p0  = m_transToWorld * Point3(0, 0, 0);
   const Point3 &p1  = m_transToWorld * Point3(1, 0, 0);
   const Point3 &p2  = m_transToWorld * Point3(0, 1, 0);
   const Point3 &p3  = m_transToWorld * Point3(0, 0, 1);
   
   const Vector3 &b1 = (p1 - p0);
   const Vector3 &b2 = (p2 - p0);
   const Vector3 &b3 = (p3 - p0);
   
   real_t area = 0;
   area += _parallelogramArea(b1, b2);
   area += _parallelogramArea(b1, b3);
   area += _parallelogramArea(b2, b3);
   area += _parallelogramArea(b2, b1);
   area += _parallelogramArea(b3, b2);
   area += _parallelogramArea(b3, b1);
   
   return area;
}

#if 0
Vector3 Cube::getNormal(const Point3 &pt) const {
   const Point3 &ptObj = m_transToWorldInv * pt;
   Vector3 nObj, nWorld;
   
   for(int i = 3; i--;) {
      const real_t &val = ptObj[i];
      
      if (EQ(ABS(val), 0.5)) {
         nObj[i] = 1 - 2 * (val < 0);
         _transformVector3ObjToWorld(nObj, nWorld);
         
         return nWorld;
      }
   }
   
   ASSERT(0 && "invalid Cube::getNormal");
   
   nObj[0] = 1;
   _transformVector3ObjToWorld(nObj, nWorld);
   
   return nWorld;
}

#endif

}

