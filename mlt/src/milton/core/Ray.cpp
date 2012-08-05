/**<!-------------------------------------------------------------------->
   @file   Ray.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      3-Dimensional Ray (origin + direction), including several 
   optimization fields (inverse direction and "mailbox ID")
   <!-------------------------------------------------------------------->**/

#include "Ray.h"

namespace milton {
   unsigned int Ray::s_rayID;
}

