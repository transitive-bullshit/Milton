/**<!-------------------------------------------------------------------->
   @file   SpatialAccel.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Generic interface for intersection acceleration data structures used 
   for ray visibility testing and sampling during rendering.
   <!-------------------------------------------------------------------->**/

#include "SpatialAccel.h"
#include <GL/gl.h>

namespace milton {

void SpatialAccel::preview() {
   m_aabb.preview();
}

}

