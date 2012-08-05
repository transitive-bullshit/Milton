/**<!-------------------------------------------------------------------->
   @file   Transformable.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a Shape which is defined in its own local "object space" and
   knows how to transform itself into the global "world space"
   <!-------------------------------------------------------------------->**/

#include "Transformable.h"
#include <GL/gl.h>

namespace milton {

void Transformable::preview() {
   real_t matrix[16];
   
   m_transToWorld.fillGLMatrix(matrix);
   glMultMatrixreal_t(matrix);
   
   Shape::preview();
}

Point3 Transformable::getPosition(const UV &uv) {
   return m_transToWorld * Point3();
}

void Transformable::init() {
   m_aabb = m_objSpaceAABB.getTransformed(m_transToWorld);
   
   Shape::init();
}

}

