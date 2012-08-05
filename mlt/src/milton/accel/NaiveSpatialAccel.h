/**<!-------------------------------------------------------------------->
   @class  NaiveSpatialAccel
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Naive/identity spatial acceleration (no acceleration) which runs through
   all primitives linearly when testing for intersections
   <!-------------------------------------------------------------------->**/

#ifndef NAIVE_SPATIAL_ACCEL_H_
#define NAIVE_SPATIAL_ACCEL_H_

#include "accel/SpatialAccel.h"

namespace milton {

class MILTON_DLL_EXPORT NaiveSpatialAccel : public SpatialAccel {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      explicit NaiveSpatialAccel()
         : SpatialAccel()
      { }
      
      virtual ~NaiveSpatialAccel()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual bool intersects(const Ray &ray, real_t tMax = INFINITY);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // NAIVE_SPATIAL_ACCEL_H_

