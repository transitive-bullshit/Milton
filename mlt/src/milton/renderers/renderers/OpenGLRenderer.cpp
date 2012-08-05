/**<!-------------------------------------------------------------------->
   @file   OpenGLRenderer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      OpenGL preview renderer
   <!-------------------------------------------------------------------->**/

#include "OpenGLRenderer.h"
#include <Camera.h>
#include <Scene.h>
#include <GL/gl.h>

namespace milton {

void OpenGLRenderer::render() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   glMatrixMode(GL_PROJECTION);
   real_t proj[16];
   
   ASSERT(m_camera);
   m_camera->fillGLMatrix(proj);
   glLoadMatrixreal_t(proj);
   
   glMatrixMode(GL_MODELVIEW);
   ASSERT(m_scene);
   m_scene->preview();
}

}

