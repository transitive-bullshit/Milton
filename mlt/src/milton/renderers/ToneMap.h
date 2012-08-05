/**<!-------------------------------------------------------------------->
   @class  ToneMap
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      A ToneMap converts radiance values stored in an HDRImage to displayable 
   sRGB pixels in an RgbaImage.
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_TONE_MAP_H_
#define MILTON_TONE_MAP_H_

#include <utils/PropertyMap.h>

namespace milton {

class RgbaImage;
class HDRImage;

class MILTON_DLL_EXPORT ToneMap : public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline ToneMap()
      { }
      
      virtual ~ToneMap()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Static factory
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Creates and returns a ToneMap of the given type, initialized with 
       * parameters from the given PropertyMap @p p.
       */
      static ToneMap *create(const std::string &type, PropertyMap &p);
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization which may be necessary before using 
       * this ToneMap, including parsing user parameters from this ToneMap's 
       * PropertyMap
       * 
       * @note
       *    Default implementation is empty
       */
      virtual void init()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a tonemapped version of the given HDR @p input image
       */
      virtual RgbaImage *map(const HDRImage *input) = 0;
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // MILTON_TONE_MAP_H_

