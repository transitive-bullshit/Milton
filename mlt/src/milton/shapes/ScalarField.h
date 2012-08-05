/**<!-------------------------------------------------------------------->
   @class  ScalarField
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      Represents a possibly bounded 3D scalar field (function from R^3 -> R 
   which may be non-zero only within a bounded domain -- its AABB or 
   Axis-Aligned-Bounding-Box)
   <!-------------------------------------------------------------------->**/

#ifndef SCALAR_FIELD_H_
#define SCALAR_FIELD_H_

#include <common/math/Point.h>
#include <accel/AABB.h>

namespace milton {

class MILTON_DLL_EXPORT ScalarField {
   public:
      /**
       * @returns the bounding box of influence which bounds the domain in 
       *    which this ScalarField may have a non-zero value
       */
      virtual AABB getAABB() const = 0;
      
      /**
       * @returns the value of this scalar field at the given point
       * 
       * @note
       *    If the given point is outside of the field's AABB, evaluate is 
       * assumed to return zero
       */
      virtual real_t evaluate(const Point3 &pt) const = 0;
};

}

#endif // SCALAR_FIELD_H_

