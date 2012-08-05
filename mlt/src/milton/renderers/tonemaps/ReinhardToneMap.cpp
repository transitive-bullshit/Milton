/**<!-------------------------------------------------------------------->
   @file   ReinhardToneMap.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      A ToneMap converts radiance values stored in an HDRImage to displayable 
   sRGB pixels in an RgbaImage.
   
   @note for more info, see 
      Photographic Tone Reproduction for Digital Images. Reinhard et al, 2002
   <!-------------------------------------------------------------------->**/

#include "ReinhardToneMap.h"
#include <common/image/miltonimage.h>
#include <ColorUtils.h>
using namespace std;

namespace milton {

void ReinhardToneMap::init() {
   // TODO
}

RgbaImage *ReinhardToneMap::map(const HDRImage *input) {
   ASSERT(input);
   
   // create temporary, working image and an output image
   HDRImage  *image   = new HDRImage (*input);
   RgbaImage *out     = new RgbaImage(input->getWidth(), input->getHeight());
   RgbaHDR *imageData = image->getData();
   Rgba32 *outData    = out->getData();
   const unsigned n   = image->getSize();
   
   real_t exposure = 0.18;
   real_t white    = 0;
   real_t key      = 0;
   
   { // convert to xyY color space and find image key
      for(unsigned i = n; i--;) {
         RgbaHDR &p = imageData[i];
         
         // convert RGB to XYZ
         RGBtoXYZ(p.r, p.g, p.b, p.r, p.g, p.b);
         ASSERT(p.g >= 0);
         
         // convert XYZ to xyY to remove X and Z dependence on luminance
         //XYZtoxyY(p.r, p.g, p.b, p.r, p.b, p.g);
         const RgbaHDR temp = p;
         p.r = temp.r / (temp.r + temp.g + temp.b);
         p.b = temp.g / (temp.r + temp.g + temp.b);
         
         key  += log(EPSILON + p.g);
         white = MAX(white, p.g);
      }
      
      key   = exp(key / ((real_t)n));
      white = (white <= 0 ? 0 : 1.0 / (white * white));
   }
   
   { // scale luminance, convert back to RGB, and perform gamma correction
      const real_t scale = (key <= 0 ? 0 : exposure / key);
      const real_t gamma = 2.22;
      const real_t correction = 1.0 / gamma;
      
      for(unsigned i = n; i--;) {
         RgbaHDR &p = imageData[i];
         Rgba32  &o = outData[i];
         
         const real_t lum = scale * p.g;
         p.g = lum * (1.0 + lum * white) / (1.0 + lum);
         
         const RgbaHDR temp = p;
         p.r = temp.g * (temp.r) / temp.b;
         p.b = temp.g * (1 - temp.r - temp.b) / temp.b;
         
         // note: this also works fairly well
         //p.r *= p.g / old;
         //p.b *= p.g / old;
         
         // convert xyY to XYZ to RGB
         //xyYtoXYZ(p.r, p.b, p.g, p.r, p.g, p.b);
         XYZtoRGB(p.r, p.g, p.b, p.r, p.g, p.b);
         
         p.r = 255 * pow(p.r, correction);
         p.g = 255 * pow(p.g, correction);
         p.b = 255 * pow(p.b, correction);
         
         o.r = (unsigned char) CLAMP(p.r, 0, 255);
         o.g = (unsigned char) CLAMP(p.g, 0, 255);
         o.b = (unsigned char) CLAMP(p.b, 0, 255);
         o.a = 255;
      }
   }
   
   safeDelete(image);
   return out;
}

}

