/**<!-------------------------------------------------------------------->
   @file   Image.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
     Abstract image class supporting a standard 32-bit Rgba pixel format, as 
   well as an extended, 128- or 256-bit floating-point RgbaHDR pixel format 
   for high dynamic range images
   
   @note
      In addition to Qt's normal image loading / saving, represented by the 
   following table, this class also supports loading / saving of images in 
   HDR, OpenEXR, and PFM image formats.
   
   @htmlonly
   <pre>
   Default / built-in image formats supported by Qt:
   
    Format                 Description                     Qt Support
    ------  -------------------------------------------  --------------
      BMP   Windows Bitmap                               Read / Write
      GIF   Graphic Interchange Format (optional)        Read
      JPG   Joint Photographic Experts Group             Read / Write
      JPEG  Joint Photographic Experts Group             Read / Write
      PNG   Portable Network Graphics                    Read / Write
      PBM   Portable Bitmap                              Read
      PGM   Portable Graymap                             Read
      PPM   Portable Pixmap                              Read / Write
      TIFF  Tagged Image File Format                     Read / Write
      XBM   X11 Bitmap                                   Read / Write
      XPM   X11 Pixmap                                   Read / Write
   
   Extra High Dynamic Range image formats supported by Milton:
   
    Format                 Description                      Support
    ------  -------------------------------------------   ----------- 
      HDR   Also known as RGBE (Radiance, by Greg Ward)  Read / Write
      EXR   ILM's OpenEXR Format                         Read / Write*
      PFM   Portable Float Map Format                    Read / Write
   
   </pre>
   @endhtmlonly
   
   @note
      * OpenEXR is only supported on platforms / builds with ILM's OpenEXR 
   libraries which do not come bundled with Milton.
   <!-------------------------------------------------------------------->**/

#include "Image.h"
#include "HDRUtils.h"
#include "HDRImage.h"
#include "RgbaImage.h"

#include <QtGui/QImage>
#include <algorithm>
#include <cctype>

namespace milton {

struct Lower {
   int operator()(int c) {
       return std::tolower(c);
   }
};

Image *Image::load(const std::string &filename) {
   std::string suffix = filename.substr(filename.rfind('.') + 1);
   std::transform(suffix.begin(), suffix.end(), suffix.begin(), Lower());
   
   if (suffix == "hdr" || suffix == "rgbe") {
      return HDRUtils::loadHDR(filename);
   } else if (suffix == "exr") {
      return HDRUtils::loadEXR(filename);
   } else if (suffix == "pfm") {
      return HDRUtils::loadPFM(filename);
   } else { // default to Qt's image loading capabilities
      QImage image;
      if (!image.load(QString(filename.c_str())))
         return NULL;
      
      if (image.format() != QImage::Format_RGB32)
         image = image.convertToFormat(QImage::Format_RGB32);
      
      const unsigned width  = image.width();
      const unsigned height = image.height();
      
      RgbaImage *rgbaImage = new RgbaImage(width, height);
      Rgba32 *data = rgbaImage->getData();
      QRgb *qrgb   = (QRgb *)image.bits();
      
      // convert from ARGB to RGBA 
      for(unsigned i = width * height; i--;) {
         const QRgb &argb = qrgb[i];
         
         data[i] = Rgba32(qRed(argb), qGreen(argb), qBlue(argb));
      }
      
      return rgbaImage;
   }
}

bool Image::save(const std::string &filename) const {
   std::string suffix = filename.substr(filename.rfind('.') + 1);
   std::transform(suffix.begin(), suffix.end(), suffix.begin(), Lower());
   
   if (suffix == "hdr" || suffix == "rgbe") {
      return HDRUtils::saveHDR(filename, this);
   } else if (suffix == "exr") {
      return HDRUtils::saveEXR(filename, this);
   } else if (suffix == "pfm") {
      return HDRUtils::savePFM(filename, this);
   } else {
      QImage image(m_width, m_height, QImage::Format_RGB32);
      QRgb *qrgb = (QRgb *)image.bits();
      
      // convert from RGBA to ARGB
      for(unsigned i = m_height; i--;) {
         for(unsigned j = m_width; j--;) {
            const Rgba32 &rgba32 = getPixel<Rgba32>(i, j);
            
            qrgb[i * m_width + j] = 
               qRgba(rgba32.r, rgba32.g, rgba32.b, rgba32.a);
         }
      }
      
      return image.save(QString(filename.c_str()), NULL, -1);
   }
}

}

