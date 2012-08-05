/**<!-------------------------------------------------------------------->
   @file   Camera.cpp
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract camera that all Milton cameras subclass, representing a 
   mapping from film-space to world-space and vice-versa. Cameras may 
   optionally also specify an OpenGL preview projection matrix that can be 
   used by OpenGLRenderer.
      Film-space is represented by (slightly modified) Normalized Device 
   Coordinates (NDC), which are unit film-plane coordinates, with (x,y) both 
   ranging inbetween 0 and 1. NDC lie in the unit square [0,1]^2, where [0,0] 
   gets mapped to the upper left corner of the corresponding Viewport/Image, 
   and [1,1] is the lower right corner. NDC film-space allows for mapping 
   rendering results onto an arbitrarily-sized Viewport/Image. 
   
   @see ThinLensCamera
   <!-------------------------------------------------------------------->**/

#include "Camera.h"
#include <SurfacePoint.h>
#include <Viewport.h>

namespace milton {

Point2 Camera::getProjection(const Point3 &p, const Viewport &v) const {
   const Point2 &NDC = getProjection(p);
   unsigned outX = 0, outY = 0;
   
   v.getBin(NDC, outX, outY);
   return Point2(outX, outY);
}

Point3 Camera::getPosition(const UV &uv) {
   return getWorldRay(Point2(0.5, 0.5)).origin;
}

void Camera::_getUV(SurfacePoint &pt) const {
   NYI();
}

void Camera::_getGeometricNormal(SurfacePoint &pt) const {
   pt.normalG = getWorldRay(Point2(pt.uv.u, pt.uv.v)).direction;
}

}

