/**<!-------------------------------------------------------------------->
   @class  SpatialAccel
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Generic interface for intersection acceleration data structures used 
   for ray visibility testing and sampling during rendering.
   <!-------------------------------------------------------------------->**/

#ifndef SPATIAL_ACCEL_H_
#define SPATIAL_ACCEL_H_

#include <utils/PropertyMap.h>
#include <shapes/Intersectable.h>
#include <vector>

namespace milton {

class MILTON_DLL_EXPORT SpatialAccel : public PropertyMap, public SSEAligned {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline SpatialAccel() 
         : m_primitives(NULL)
      { }
      
      virtual ~SpatialAccel() 
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization Routines
      //@{-----------------------------------------------------------------
      
      virtual void setGeometry(IntersectableList *primitives) {
         m_primitives = primitives;
      }
      
      /// Initializes the AABB surrounding all of the geometry
      virtual void init() {
         ASSERT(m_primitives);
         m_aabb = AABB();
         
         FOREACH(IntersectableListConstIter, *m_primitives, iter) {
            const Intersectable *shape = *iter;
            
            const AABB &aabb = shape->getAABB();
            ASSERT(aabb.isValid());
            
            if (!aabb.isPoint())
               m_aabb.add(aabb);
         }
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt) = 0;
      
      virtual bool intersects(const Ray &ray, real_t tMax = INFINITY) = 0;
      
      virtual void preview();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      inline const IntersectableList *getIntersectables() const {
         return m_primitives;
      }
      
      inline const AABB &getAABB() const {
         return m_aabb;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      AABB               m_aabb;
      IntersectableList *m_primitives;
};

}

#endif // SPATIAL_ACCEL_H_

