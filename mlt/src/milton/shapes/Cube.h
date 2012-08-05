/**<!-------------------------------------------------------------------->
   @class  Cube
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a unit cube in 3-space from (-.5,-.5,-.5) to (.5,.5,.5)
   <!-------------------------------------------------------------------->**/

#ifndef CUBE_H_
#define CUBE_H_

#include <shapes/Implicit.h>

namespace milton {

class MILTON_DLL_EXPORT Cube : public Implicit {
   public:
      ///@name Intersection routines
      //@{-----------------------------------------------------------------
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual bool intersects(const Ray &ray, real_t tMax = INFINITY);
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      virtual void preview();
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
      
      /**
       * @returns the total surface area of this shape
       */
      virtual real_t _getSurfaceArea();
      
      inline real_t _parallelogramArea(const Vector3 &b1, const Vector3 &b2) {
         return b1.cross(b2).getMagnitude();
         // area = |b1| * |b2| * sin(theta), theta = acos(b1.norm, b2.norm)
         //return b1.getMagnitude() * b2.getMagnitude() * 
         //   sin(acos(b1.getNormalized().dot(b2.getNormalized())));
      }
};

}

#endif // CUBE_H_

