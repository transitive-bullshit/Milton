/**<!-------------------------------------------------------------------->
   @class  NullToneMap
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      A ToneMap converts radiance values stored in an HDRImage to displayable 
   sRGB pixels in an RgbaImage.
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_NULL_TONE_MAP_H_
#define MILTON_NULL_TONE_MAP_H_

#include <renderers/ToneMap.h>

namespace milton {

class MILTON_DLL_EXPORT NullToneMap : public ToneMap {
   public:
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual RgbaImage *map(const HDRImage *input);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // MILTON_NULL_TONE_MAP_H_

