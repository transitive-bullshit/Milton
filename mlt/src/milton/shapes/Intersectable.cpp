/**<!-------------------------------------------------------------------->
   @file   Intersectable.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Interface for an intersectable object in 3-space, with some extra 
   functionality specific to accelerating spatial intersection queries
   <!-------------------------------------------------------------------->**/

#include "Intersectable.h"
#include <SurfacePoint.h>

namespace milton {

void Intersectable::init() {
   // intentionally empty
}

bool Intersectable::intersects(const Ray &ray, real_t tMax) {
   SurfacePoint pt;
   const real_t t = getIntersection(ray, pt);
   
   return (t < tMax - EPSILON && t > EPSILON);
}

}

