/**<!-------------------------------------------------------------------->
   @file   GLState.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Utility wrapper classes for accessing and synchronizing OpenGL state
   in a cleaner, more object-oriented manner, grouping similar state items 
   with each other.
   
   @note
      Where applicable, acceptable parameters are listed along with default 
   values marked between '<' and '>'. For example, the frontFace property of
   GLPrimitiveStateItem is preceded by the following comment:
      /// glFrontFace; GL_CW or <GL_CCW>
   Where GL_CCW is the OpenGL-defined default value for this property.
   <!-------------------------------------------------------------------->**/

#include "GLState.h"
#include <GL/glu.h>
using namespace std;

namespace milton {

// TODO: add GLPixelTransferStateItem?
// TODO: add GLClearValueStateItem?

int GLState::getFreeLight() {
   GLint maxNoLights = 0;
   glGetIntegerv(GL_MAX_LIGHTS, &maxNoLights);
   
   for(int i = 0; i < maxNoLights; ++i) {
      GLboolean isEnabled = false;
      GLint lightIndex = GL_LIGHT0 + i;
      
      glGetBooleanv(lightIndex, &isEnabled);
      if (!isEnabled)
         return lightIndex;
   }
   
   return -1;
}

bool GLState::glCheckError(const char *file, int line) {
   bool retVal = true;
   GLenum glErr;
   
   while ((glErr = glGetError()) != GL_NO_ERROR) {
      cerr << file << ":" << line << ": " << gluErrorString(glErr);
      cerr << endl;
      
      retVal = false;
   }
   
   if (!retVal) {
      cerr << glErr << endl;
      ASSERT(0);
   }
   
   return retVal;
}

}

