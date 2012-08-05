/**<!-------------------------------------------------------------------->
   @file   Implicit.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract representation of an implicit shape in 3-space.
   <!-------------------------------------------------------------------->**/

#include "Implicit.h"

namespace milton {

void Implicit::init() {
   m_objSpaceAABB = AABB(Vector3(-.5, -.5, -.5), Vector3(.5, .5, .5));
   
   Transformable::init();
}

}

