/**<!-------------------------------------------------------------------->
   @file   Cone.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a vertical 3D cone enclosed in the unit box 
   (-.5,-.5,-.5) to (.5,.5,.5) -- the tip is at (0,.5,0)
   <!-------------------------------------------------------------------->**/

#include "Cone.h"
#include "SurfacePoint.h"

namespace milton {

real_t Cone::getIntersection(const Ray &ray, SurfacePoint &pt) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   // A = (Dx^2 + Dz^2 - 1/4 * Dy^2)
   const real_t A = 2 * (D[0] * D[0] + D[2] * D[2] - 0.25 * D[1] * D[1]);

   // B = (2 * Px * Dx + 2 * Pz * Dz - 1/2 * Py * Dy + 1/4 * Dy)
   const real_t B = (2 * P[0] * D[0] + 2 * P[2] * D[2] - 0.5 * P[1] * D[1] + 0.25 * D[1]);

   // C = (Px^2 + Pz^2 - 1/4 * Py^2 + 1/4 * Py - 1/16)
   const real_t C = (P[0] * P[0] + P[2] * P[2] - 0.25 * P[1] * P[1] + 0.25 * P[1] - 0.0625);

   // Discriminant of quadratic = B^2 - 4 * A * C
   real_t discriminant =  B * B - 2 * A * C;
   real_t minT = INFINITY;

   // Check for intersection with Cone Body
   // -------------------------------------
   if (discriminant >= 0) {
      discriminant = sqrt(discriminant);
      
      const real_t t1 = (-B + discriminant) / A;
      const Point3 &p1 = P + t1 * D;
      if (p1[2] < 0.5 && p1[2] > -0.5 && p1[1] > -0.5 && p1[1] < 0.5 && t1 > INTERSECT_TOLERANCE)
         minT = t1;

      const real_t t2 = (-B - discriminant) / A;

      if (isValidT(t2, minT)) {
         const Point3 &p2 = P + t2 * D;
         
         if (p2[2] < 0.5 && p2[2] > -0.5 && p2[1] > -0.5 && p2[1] < 0.5)
            minT = t2;
      }
   }
   
   // Check for intersection with Cone Cap
   // ------------------------------------
   const real_t tCap = (-0.5 - P[1]) / D[1];
   
   pt.normalCase = 0;
   if (isValidT(tCap, minT)) {
      const Point3 &intersection3 = P + tCap * D;
      // For Cone Cap: x^2 + z^2 <= (1/2)^2  must hold for valid intersection point
      
      if (intersection3[0] * intersection3[0] +
          intersection3[2] * intersection3[2] <= 0.25) {
         pt.normalCase = 1;
         return tCap;
      }
   }
   
   return minT;
}

bool Cone::intersects(const Ray &ray, real_t tMax) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   // Check for intersection with Cone Cap
   // ------------------------------------
   const real_t tCap = (-0.5 - P[1]) / D[1];
   
   if (isValidT(tCap, tMax)) {
      const Point3 &intersection3 = P + tCap * D;
      
      // For Cone Cap: x^2 + z^2 <= (1/2)^2  must hold for valid intersection point
      if (intersection3[0] * intersection3[0] + intersection3[2] * intersection3[2] <= 0.25)
         return true;
   }
   
   // A = (Dx^2 + Dz^2 - 1/4 * Dy^2)
   const real_t A = 2 * (D[0] * D[0] + D[2] * D[2] - 0.25 * D[1] * D[1]);
   
   // B = (2 * Px * Dx + 2 * Pz * Dz - 1/2 * Py * Dy + 1/4 * Dy)
   const real_t B = (2 * P[0] * D[0] + 2 * P[2] * D[2] - 0.5 * P[1] * D[1] + 0.25 * D[1]);
   
   // C = (Px^2 + Pz^2 - 1/4 * Py^2 + 1/4 * Py - 1/16)
   const real_t C = (P[0] * P[0] + P[2] * P[2] - 0.25 * P[1] * P[1] + 0.25 * P[1] - 0.0625);
   
   // Discriminant of quadratic = B^2 - 4 * A * C
   real_t discriminant =  B * B - 2 * A * C;
   
   // Check for intersection with Cone Body
   // -------------------------------------
   if (discriminant >= 0) {
      discriminant = sqrt(discriminant);
      
      const real_t t1 = (-B + discriminant) / A;
      if (isValidT(t1, tMax)) {
         const Point3 &p1 = P + t1 * D;
         
         if (p1[2] < 0.5 && p1[2] > -0.5 && p1[1] > -0.5 && p1[1] < 0.5)
            return true;
      }
      
      const real_t t2 = (-B - discriminant) / A;
      if (isValidT(t2, tMax)) {
         const Point3 &p2 = P + t2 * D;
         return (p2[2] < 0.5 && p2[2] > -0.5 && p2[1] > -0.5 && p2[1] < 0.5);
      }
   }
   
   return false;
}

void Cone::_getUV(SurfacePoint &pt) const {
   const Point3 &P = pt.position;
   real_t &u = pt.uv.u, &v = pt.uv.v;
   
   if (pt.normalCase) {  // Cylinder or Cone Cap
      v = P[2] + 0.5;
      u = P[0] + 0.5;
      
      if (pt.normalCase == 1)
         v = 1 - v;
   } else {  // Cylinder or Cone Body
      u = 1 - atan2(P[2], P[0]) * (1.0 / (2 * M_PI));
      v = 0.5 - P[1];  // height up body
   }
}

void Cone::_getGeometricNormal(SurfacePoint &pt) const {
   Vector3 nObj;
   
   if (pt.normalCase) {
      nObj[1] = 1 - ((pt.normalCase == 1) * 2);
   } else {
      const Point3 &P = pt.position;
      const real_t m  = 1 / sqrt(P[0] * P[0] + P[2] * P[2]);
      
      nObj = Vector3(P[0] * m, .5, P[2] * m);
   }
   
   _transformVector3ObjToWorld(nObj, pt.normalG);
}

void Cone::preview() {
   Transformable::preview();
   
   if (m_quadric == NULL)
      m_quadric = gluNewQuadric();
   
   ASSERT(m_quadric != NULL);
   
   glRotated(-90, 1, 0, 0);
   glTranslated(0, 0, -.5);
   gluCylinder(m_quadric, 0.5, 0.0, 1, 20, 20);
}

}

