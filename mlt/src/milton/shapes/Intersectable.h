/**<!-------------------------------------------------------------------->
   @class  Intersectable
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Interface for an intersectable object in 3-space, with some extra 
   functionality specific to accelerating spatial intersection queries
   <!-------------------------------------------------------------------->**/

#ifndef INTERSECTABLE_H_
#define INTERSECTABLE_H_

#include <accel/AABB.h>

namespace milton {

struct Ray;
struct SurfacePoint;
class  Intersectable;

DECLARE_STL_TYPEDEF(std::vector<Intersectable*>, IntersectableList);

class MILTON_DLL_EXPORT Intersectable {
   public:
      ///@name Fast-access public data
      //@{-----------------------------------------------------------------
      
      /// for 'mailboxing' intersection tests to speed up traversal of 
      /// kdTreeAccel by ensuring that large primitives which may lie in 
      /// more than one voxel are only tested for intersection once-per-ray
      /// for more info, search the raytracing literature for mailboxing
      unsigned int rayID;
      
      
      //@}-----------------------------------------------------------------
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   Intersectable()
         : rayID((unsigned) -1)
      { }
      
      virtual ~Intersectable() 
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes this Intersectable for fast intersection testing
       * 
       * @note default implementation is empty
       * @note must be called before getIntersection or intersects
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Tests the given ray with this object for intersection. If a valid 
       * intersection exists, its "t" value will be returned and the given 
       * SurfacePoint will be initialized with enough data for this object to 
       * later fill in all relevant information lazily (world space normal 
       * at intersection point, uv coordinates, etc.)
       * 
       * @note if you only care about whether or not an intersection exists 
       *    and don't actually need to know the exact intersection point, 
       *    see the intersects method instead.
       * @returns the smallest positive "t" value of any intersections 
       *    found, or INFINITY if no valid intersection exists
       */
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt) = 0;
      
      /**
       * @brief
       *    Used for occlusion/visibility testing where you don't necessarily 
       * care about anything other than whether or not an intersection exists.
       * (generally faster than getIntersection)
       * 
       * @note default implementation defers to getIntersection
       * @returns whether or not the given ray intersects this object with a 
       *    positive "t" value greater than EPSILON and less than the given 
       *    @p tMax
       */
      virtual bool intersects(const Ray &ray, real_t tMax = INFINITY);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      virtual AABB getAABB() const = 0;
      
      inline real_t getMin(unsigned dim) const {
         return getAABB().min[dim];
      }
      
      inline real_t getMax(unsigned dim) const {
         return getAABB().max[dim];
      }
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // INTERSECTABLE_H_

