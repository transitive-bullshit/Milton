/**<!-------------------------------------------------------------------->
   @class  Plane
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a unit plane in the y=0 plane from (-.5,0,-.5) 
   to (.5,0,.5)
   <!-------------------------------------------------------------------->**/

#ifndef PLANE_H_
#define PLANE_H_

#include <shapes/Implicit.h>

namespace milton {

class MILTON_DLL_EXPORT Plane : public Implicit {
   public:
      
      ///@name Initialization routines
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Precomputes world-space surface area
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Intersection routines
      //@{-----------------------------------------------------------------
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual bool intersects(const Ray &ray, 
                              real_t tMax = INFINITY);
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      virtual void preview();
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the point on the surface of this shape corresponding to the 
       *    given UV coordinates
       */
      virtual Point3 getPosition(const UV &uv); 
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
      
      /**
       * @returns the total surface area of this shape in world-space
       */
      virtual real_t _getSurfaceArea();
};

}

#endif // PLANE_H_

