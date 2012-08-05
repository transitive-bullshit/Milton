/**<!-------------------------------------------------------------------->
   @class  HDRUtils.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2009
   
   @brief
      Utilities for reading and writing HDR, OpenEXR, and PFM high dynamic 
   range image formats
   
   @note
      This code is based on similar code found in exrtools, by Billy Biggs 
   (http://scanline.ca/exrtools/)
   <!-------------------------------------------------------------------->**/

#ifndef HDR_UTILS_H_
#define HDR_UTILS_H_

#include <common/common.h>
#include <string>

namespace milton {

class HDRImage;
class RgbaImage;
class Image;

class MILTON_DLL_EXPORT HDRUtils {
   public:
      static HDRImage *loadHDR(const std::string &filename);
      static HDRImage *loadEXR(const std::string &filename);
      static HDRImage *loadPFM(const std::string &filename);
      
      static bool saveHDR(const std::string &filename, const Image *image);
      static bool saveEXR(const std::string &filename, const Image *image);
      static bool savePFM(const std::string &filename, const Image *image);
      
      static HDRImage  *toHDRImage (const RgbaImage *image);
      static RgbaImage *toRgbaImage(const HDRImage  *image);
      
   private:
      static std::string _toFullName(const std::string &filename, 
                                     const std::string &ext);
      
      static void _warnMissingEXR(const char *function, 
                                  const std::string &filename);
};

}

#endif // HDR_UTILS_H_

