/**<!-------------------------------------------------------------------->
   @class  ReinhardToneMap
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      A ToneMap converts radiance values stored in an HDRImage to displayable 
   sRGB pixels in an RgbaImage.
   
   @note for more info, see 
      Photographic Tone Reproduction for Digital Images. Reinhard et al, 2002
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_REINHARD_TONE_MAP_H_
#define MILTON_REINHARD_TONE_MAP_H_

#include <renderers/ToneMap.h>

namespace milton {

class MILTON_DLL_EXPORT ReinhardToneMap : public ToneMap {
   public:
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization which may be necessary before using 
       * this ToneMap, including parsing user parameters from this ToneMap's 
       * PropertyMap
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual RgbaImage *map(const HDRImage *input);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // MILTON_REINHARD_TONE_MAP_H_

