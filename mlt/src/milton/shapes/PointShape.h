/**<!-------------------------------------------------------------------->
   @class  PointShape
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a single point in 3-space (used to represent point 
   lights and pinhole cameras)
   <!-------------------------------------------------------------------->**/

#ifndef POINT_SHAPE_H_
#define POINT_SHAPE_H_

#include <shapes/Implicit.h>

namespace milton {

class MILTON_DLL_EXPORT PointShape : public Transformable {
   public:
      ///@name Intersection routines
      //@{-----------------------------------------------------------------
      
      /// always returns an invalid intersection
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      /// @returns false
      virtual bool intersects(const Ray &ray, 
                              real_t tMax = INFINITY);
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      virtual void preview();
      
      virtual bool hasNormal() const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the total surface area of this shape
       */
      virtual real_t getSurfaceArea() {
         return 0;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
};

}

#endif // POINT_SHAPE_H_

