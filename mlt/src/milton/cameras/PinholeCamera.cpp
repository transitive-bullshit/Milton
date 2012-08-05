/**<!-------------------------------------------------------------------->
   @file   PinholeCamera.cpp
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

#include "PinholeCamera.h"
#include <algebra.h>

namespace milton {

void PinholeCamera::init(Scene *scene) {
   insert("aperture", 0);
   
   ThinLensCamera::init(scene);
}

}

