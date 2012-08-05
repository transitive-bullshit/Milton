/**<!-------------------------------------------------------------------->
   @file:   AABB.inl
   @author: Travis Clayton Fischer
   @acct:   tfischer
   @date:   Tue Sep 30 20:35:43 2008
<!-------------------------------------------------------------------->**/

#ifndef AABB_INL_
#define AABB_INL_

#include <core/Ray.h>

namespace milton {

/**
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 * 
 * Modified slightly by for the purposes of this program
 */
inline bool AABB::intersects(const Ray &ray, real_t &tMin, real_t &tMax) const {
   const Vector3 bounds[2] = { min, max };
   const bool xSign = (ray.invDir[0] < 0);
   const bool ySign = (ray.invDir[1] < 0);
   
   tMin  = (bounds[xSign ][0] - ray.origin[0]) * ray.invDir[0];
   tMax  = (bounds[!xSign][0] - ray.origin[0]) * ray.invDir[0];
   const real_t tyMin = (bounds[ySign ][1] - ray.origin[1]) * ray.invDir[1];
   const real_t tyMax = (bounds[!ySign][1] - ray.origin[1]) * ray.invDir[1];
   
   if ((tMin > tyMax) || (tyMin > tMax))
      return false;
   
   if (tyMin > tMin)
      tMin = tyMin;
   if (tyMax < tMax)
      tMax = tyMax;
   
   const bool zSign   = (ray.invDir[2] < 0);
   const real_t tzMin = (bounds[zSign ][2] - ray.origin[2]) * ray.invDir[2];
   const real_t tzMax = (bounds[!zSign][2] - ray.origin[2]) * ray.invDir[2];
   
   if ((tMin > tzMax) || (tzMin > tMax))
      return false;
   
   if (tzMin > tMin)
      tMin = tzMin;
   if (tzMax < tMax)
      tMax = tzMax;
   
   return (tMin <= tMax);
}

inline void AABB::add(const Vector3 &v) {
   min = Vector3::min(min, v);
   max = Vector3::max(max, v);
}

inline void AABB::add(const Point3 &p) {
   add(Vector3(p.data));
}

inline void AABB::add(const AABB &aabb) {
   if (aabb.min != aabb.max) {
      min = Vector3::min(min, aabb.min);
      max = Vector3::max(max, aabb.max);
   }
}

inline bool AABB::intersects(const AABB &aabb) const {
   const Vector3 &min2 = aabb.min, &max2 = aabb.max;
   
   return (max[0] >= min2[0] && min[0] <= max2[0] && 
           max[1] >= min2[1] && min[1] <= max2[1] &&
           max[2] >= min2[2] && min[2] <= max2[2]);
}

inline bool AABB::contains(const Point3 &p) const {
   return (max[0] >= p[0] && min[0] <= p[0] && 
           max[1] >= p[1] && min[1] <= p[1] &&
           max[2] >= p[2] && min[2] <= p[2]);
}

inline Vector3 AABB::getDiagonal() const {
   return (max - min);
}

inline Point3 AABB::getCenter() const {
   return Point3() + (min + max) / 2.0;
}

inline unsigned AABB::getMaxExtent() const {
   const Vector3 &diag = max - min;
   
   return (diag[0] > diag[1]) ? 
      2 * (diag[2] > diag[0]) : 
      1 + (diag[2] > diag[1]);
}

inline unsigned AABB::getMinExtent() const {
   const Vector3 &diag = max - min;
   
   return (diag[0] < diag[1]) ? 
      2 * (diag[2] < diag[0]) : 
      1 + (diag[2] < diag[1]);
}

inline real_t AABB::getSurfaceArea() const {
   const Vector3 &diag = max - min;
   
   return 2 * (diag[0] * diag[1] + 
               diag[1] * diag[2] + 
               diag[2] * diag[0]);
}

inline void AABB::getMinMaxSurfaceArea(const unsigned axis, 
                                       const real_t pos, 
                                       real_t &leftArea, 
                                       real_t &rightArea) const
{
   Vector3 diag = max - min;
   
   diag[axis]   = pos - min[axis];
   leftArea     = 2 * (diag[0] * diag[1] + 
                       diag[1] * diag[2] + 
                       diag[2] * diag[0]);
   
   diag[axis]   = max[axis] - pos;
   rightArea    = 2 * (diag[0] * diag[1] + 
                       diag[1] * diag[2] + 
                       diag[2] * diag[0]);
}

inline AABB AABB::getTransformed(const Matrix4x4 &trans) const {
   const Vector3 &p0 = 
      Vector3((trans * Point3(min[0], min[1], min[2])).data);
   const Vector3 &p1 = 
      Vector3((trans * Point3(max[0], min[1], min[2])).data);
   const Vector3 &p2 = 
      Vector3((trans * Point3(min[0], max[1], min[2])).data);
   const Vector3 &p3 = 
      Vector3((trans * Point3(max[0], max[1], min[2])).data);
   const Vector3 &p4 = 
      Vector3((trans * Point3(min[0], min[1], max[2])).data);
   const Vector3 &p5 = 
      Vector3((trans * Point3(max[0], min[1], max[2])).data);
   const Vector3 &p6 = 
      Vector3((trans * Point3(min[0], max[1], max[2])).data);
   const Vector3 &p7 = 
      Vector3((trans * Point3(max[0], max[1], max[2])).data);
   
   AABB transformed;
   
   transformed.add(p0);
   transformed.add(p1);
   transformed.add(p2);
   transformed.add(p3);
   transformed.add(p4);
   transformed.add(p5);
   transformed.add(p6);
   transformed.add(p7);
   
   return transformed;
}

inline bool AABB::isValid() const {
   return (min[0] <= max[0] && min[1] <= max[1] && min[2] <= max[2]);
}

inline bool AABB::isPoint() const {
   return (min[0] == max[0] && min[1] == max[1] && min[2] == max[2]);
}

inline std::ostream &operator<<(std::ostream &os, const AABB &aabb) {
   os << "{ min = " << aabb.min 
      << ", max = " << aabb.max 
      << ", mid = " << aabb.getCenter() 
      << " }";
   
   return os;
}

}

#endif // AABB_INL_

