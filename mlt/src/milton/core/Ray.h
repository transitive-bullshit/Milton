/**<!-------------------------------------------------------------------->
   @class  Ray
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      3-Dimensional Ray (origin + direction), including several 
   optimization fields (inverse direction and "mailbox ID")
   <!-------------------------------------------------------------------->**/

#ifndef RAY_H_
#define RAY_H_

#include <common/math/algebra.h>

namespace milton {

struct MILTON_DLL_EXPORT Ray : public SSEAligned {
    
   ///@name Public Data
   //@{-----------------------------------------------------------------
   
   Vector3 direction;
   Vector3 invDir;
   Point3  origin;
   
   
   //@}-----------------------------------------------------------------
   ///@name Constructors
   //@{-----------------------------------------------------------------
   
   inline Ray() 
   { }
   
   /**
    * @param origin_ is the origin of the ray
    * @param direction_ is the direction of the ray
    */
   inline Ray(const Point3 &origin_, const Vector3 &direction_)
      : direction(direction_), 
        invDir(direction_.getReciprocal()), 
        origin(origin_)
   { }
   
   
   //@}-----------------------------------------------------------------
   
   /**
    * @returns whether or not the given t-value represents a valid 
    *    intersection point
    * 
    * @note a ray is defined by P + t*d, where P is the ray's origin, d 
    *    is a direction vector, and t is the distance along the ray in 
    *    meters; with this definition, a valid t-value representing a 
    *    point along the ray where an intersection could have occurred 
    *    is positive (not behind the ray's origin) and not at at infinity.
    *    In practice, in addition to ensuring that an intersection point's 
    *    t-value is positive, it dramatically increases robustness to 
    *    define valid t-values as greater than an epsilon value to prevent 
    *    self-intersections when a ray's origin and intersection point 
    *    coincide and the calculated t-value is close to zero but not 
    *    necessarily equal to zero due to floating point inprecision; by 
    *    throwing out t-values that are smaller than epsilon, we safeguard 
    *    ourselves from this numerical instability and disregard 
    *    invalid self-intersections
    */
   static inline bool isValid(real_t t) {
      return (t > EPSILON && t < INFINITY);
   }
   
   private:
      static unsigned int s_rayID;
};

}

#endif // RAY_H_

