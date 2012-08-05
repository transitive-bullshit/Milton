/**<!-------------------------------------------------------------------->
   @file:   AABB.cpp
   @author: Travis Clayton Fischer
   @acct:   tfischer
   @date:   Tue Sep 30 20:35:43 2008
<!-------------------------------------------------------------------->**/

#include "AABB.h"
#include <GL/gl.h>

namespace milton {

void AABB::preview() const {
   glDisable(GL_LIGHTING);
   glBegin(GL_LINES);
   
   {
      glVertex3real_t(min[0], min[1], min[2]);
      glVertex3real_t(max[0], min[1], min[2]);
      
      glVertex3real_t(min[0], min[1], min[2]);
      glVertex3real_t(min[0], max[1], min[2]);
      
      glVertex3real_t(min[0], min[1], min[2]);
      glVertex3real_t(min[0], min[1], max[2]);
   }
   
   { 
      glVertex3real_t(max[0], min[1], min[2]);
      glVertex3real_t(max[0], max[1], min[2]);
      
      glVertex3real_t(max[0], min[1], min[2]);
      glVertex3real_t(max[0], min[1], max[2]);
   }
   
   { 
      glVertex3real_t(min[0], max[1], min[2]);
      glVertex3real_t(max[0], max[1], min[2]);
      
      glVertex3real_t(min[0], max[1], min[2]);
      glVertex3real_t(min[0], max[1], max[2]);
   }
   
   { 
      glVertex3real_t(min[0], min[1], max[2]);
      glVertex3real_t(min[0], max[1], max[2]);
      
      glVertex3real_t(min[0], min[1], max[2]);
      glVertex3real_t(max[0], min[1], max[2]);
   }
   
   { 
      glVertex3real_t(max[0], max[1], max[2]);
      glVertex3real_t(min[0], max[1], max[2]);
      
      glVertex3real_t(max[0], max[1], max[2]);
      glVertex3real_t(max[0], min[1], max[2]);
      
      glVertex3real_t(max[0], max[1], max[2]);
      glVertex3real_t(max[0], max[1], min[2]);
   }
   
   glEnd();
   glEnable(GL_LIGHTING);
}

}

