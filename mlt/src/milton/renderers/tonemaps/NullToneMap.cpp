/**<!-------------------------------------------------------------------->
   @file   NullToneMap.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      A ToneMap converts radiance values stored in an HDRImage to displayable 
   sRGB pixels in an RgbaImage.
   <!-------------------------------------------------------------------->**/

#include "NullToneMap.h"
#include <common/image/miltonimage.h>

// disable warnings for float -> char conversion
#ifdef MILTON_ARCH_WINDOWS
#  pragma warning(disable : 4244)
#endif

namespace milton {

RgbaImage *NullToneMap::map(const HDRImage *input) {
   ASSERT(input);
   RgbaImage *out = new RgbaImage(input->getWidth(), input->getHeight());
   
   const RgbaHDR *inputData = input->getData();
   Rgba32 *outData = out->getData();
   
   const real_t gamma = 2.22;
   const real_t correction = 1.0 / gamma;
   
   for(unsigned i = input->getSize(); i--;) {
      RgbaHDR p = inputData[i];
      Rgba32 &o = outData[i];
      
      p.r = 255 * pow(p.r, correction);
      p.g = 255 * pow(p.g, correction);
      p.b = 255 * pow(p.b, correction);
      
      o.r = CLAMP(p.r, 0, 255);
      o.g = CLAMP(p.g, 0, 255);
      o.b = CLAMP(p.b, 0, 255);
      o.a = 255;
   }
   
   return out;
}

}

