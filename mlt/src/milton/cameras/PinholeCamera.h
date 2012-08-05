/**<!-------------------------------------------------------------------->
   @class  PinholeCamera
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Basic pinhole camera using perspective projection (aka Camera Obscura)
   
   @note
      A pinhole camera is a special case of the thin lens model, in which the 
   camera's aperture is infinitesmally small such that all rays which pass 
   through the aperture focus at one unique location on the film plane.
   PinholeCamera, therefore, subclasses ThinLensCamera.
   <!-------------------------------------------------------------------->**/

#ifndef PINHOLE_CAMERA_H_
#define PINHOLE_CAMERA_H_

#include "cameras/ThinLensCamera.h"

namespace milton {

class MILTON_DLL_EXPORT PinholeCamera : public ThinLensCamera {
   public:
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init(Scene *scene);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // PINHOLE_CAMERA_H_

