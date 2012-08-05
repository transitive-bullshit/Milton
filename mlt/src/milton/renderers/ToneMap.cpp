/**<!-------------------------------------------------------------------->
   @file   ToneMap.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      A ToneMap converts radiance values stored in an HDRImage to displayable 
   sRGB pixels in an RgbaImage.
   <!-------------------------------------------------------------------->**/

#include "ToneMap.h"
#include <tonemaps.h>
#include <common/image/image.h>

namespace milton {

ToneMap *ToneMap::create(const std::string &type, PropertyMap &p) {
   // Parse ToneMap parameters from PropertyMap or use sensible defaults
   ToneMap *tonemap = NULL;
   
   if (type == "reinhard" || type == "default")
      tonemap = new ReinhardToneMap();
   else 
      tonemap = new NullToneMap();
   
   ASSERT(tonemap);
   tonemap->inherit(p);
   tonemap->init();
   
   return tonemap;
}

}

