/**<!-------------------------------------------------------------------->
   @file   cameras.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Convenience header which includes all public camera headers
   <!-------------------------------------------------------------------->**/

#ifndef CAMERAS_H_
#define CAMERAS_H_

// generic Camera interface
#include <cameras/Camera.h>

// concrete Camera implementations
#include <cameras/PinholeCamera.h>
#include <cameras/ThinLensCamera.h>

#endif // CAMERAS_H_

