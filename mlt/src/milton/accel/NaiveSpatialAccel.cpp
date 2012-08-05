/**<!-------------------------------------------------------------------->
   @file   NaiveSpatialAccel.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Naive/identity spatial acceleration (no acceleration) which runs through
   all primitives linearly when testing for intersections
   <!-------------------------------------------------------------------->**/

#include "NaiveSpatialAccel.h"
#include <SurfacePoint.h>

namespace milton {

real_t NaiveSpatialAccel::getIntersection(const Ray &ray, SurfacePoint &pt) {
   {
      real_t tMin, tMax;
      
      // check for intersection between ray and root node's bounding box
      if (!m_aabb.intersects(ray, tMin, tMax))
         return false;
      
      ASSERT(tMin <= tMax);
   }
   
   real_t tMin = INFINITY;
   unsigned normalCase = pt.normalCase;
   unsigned index      = pt.index;
   Shape   *shape      = pt.shape;
   
   for(unsigned i = m_primitives->size(); i--;) {
      pt.shape  = NULL;
      pt.index  = (unsigned)(-1);
      
      Intersectable *curIntersectable = (*m_primitives)[i];
      const real_t t = curIntersectable->getIntersection(ray, pt);
      
      // TODO: why not just (t < tMin) -- fails on some cases.. figure out why
      if (t > EPSILON && t < tMin) {
         //std::cerr << tMin << std::endl;
         tMin = t;
         
         shape      = (pt.shape ? pt.shape : static_cast<Shape*>(curIntersectable));
         index      = (pt.index == ((unsigned)(-1)) ? i : pt.index);
         normalCase = pt.normalCase;
      }
   }
   
   pt.shape      = shape;
   pt.normalCase = normalCase;
   pt.index      = index;
   
   return tMin;
}

bool NaiveSpatialAccel::intersects(const Ray &ray, real_t clipMax) {
   {
      real_t tMin, tMax;
      
      // check for intersection between ray and root node's bounding box
      if (!m_aabb.intersects(ray, tMin, tMax))
         return false;
      
      ASSERT(tMin <= tMax);
   }
   
   for(unsigned i = m_primitives->size(); i--;) {
      Intersectable *curIntersectable = (*m_primitives)[i];
      
      if (curIntersectable->intersects(ray, clipMax))
         return true;
   }
   
   return false;
}

}

