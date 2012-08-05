/**<!-------------------------------------------------------------------->
   @file   RgbaImage.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2008
   
   @brief
     Image class supporting a standard 32-bit Rgba pixel format
   <!-------------------------------------------------------------------->**/

#include "RgbaImage.h"
#include "HDRImage.h"
#include <QtGui/QImage>

namespace milton {

RgbaImage::RgbaImage(const RgbaImage &copy) 
   : Image(copy.m_width, copy.m_height), m_data(NULL), m_ownData(true)
{
   m_data = new Rgba32[m_size];
   
   _setData(copy.getData());
}

RgbaImage::RgbaImage(QImage &image)
   : Image(image.width(), image.height()), m_ownData(false)
{
   if (image.format() != QImage::Format_RGB32)
      image = image.convertToFormat(QImage::Format_RGB32);
   
   m_data = (Rgba32 *)image.bits();
   
#if 0
   QRgb *qrgb = (QRgb *)image.bits();
   m_data     = new Rgba32[m_size];
   
   // convert from ARGB to RGBA 
   for(unsigned i = m_size; i--;) {
      const QRgb &argb = qrgb[i];
      
      m_data[i] = Rgba32(qRed(argb), qGreen(argb), qBlue(argb));
   }
#endif
}

RgbaImage::~RgbaImage() {
   if (m_ownData)
      safeDeleteArray(m_data);
}

bool RgbaImage::isHDR() const {
   return false;
}

real_t RgbaImage::getLuminance(unsigned row, unsigned col) const {
   ASSERT(row < m_height && col < m_width);
   
   return m_data[row * m_width + col].luminance();
}

Rgba32 RgbaImage::_getPixelRgba32 (unsigned row, unsigned col) const {
   return m_data[row * m_width + col];
}

RgbaHDR RgbaImage::_getPixelRgbaHDR(unsigned row, unsigned col) const {
   return static_cast<RgbaHDR>(m_data[row * m_width + col]);
}

void RgbaImage::_setPixel(unsigned row, unsigned col, const Rgba32  &val) {
   m_data[row * m_width + col] = val;
}

void RgbaImage::_setPixel(unsigned row, unsigned col, const RgbaHDR &val) {
   m_data[row * m_width + col] = static_cast<Rgba32>(val);
}

void RgbaImage::setSize(unsigned width, unsigned height) {
   if (m_width != width || m_height != height) {
      m_size   = width * height;
      m_width  = width;
      m_height = height;
      
      if (m_ownData) {
         Rgba32 *data = m_data;
         m_data = new Rgba32[m_size];
         
         safeDeleteArray(data);
      }
   }
}

void RgbaImage::_setData(const Rgba32 *data) {
   ASSERT(m_data);
   ASSERT(data);
   
   memcpy(m_data, data, sizeof(Rgba32) * m_size);
}

void RgbaImage::_setData(const RgbaHDR *data) {
   ASSERT(m_data);
   ASSERT(data);
   
   for(unsigned i = m_height; i--;)
      for(unsigned j = m_width; j--;)
         _setPixel(i, j, data[i * m_width + j]);
}

Image *RgbaImage::clone() const {
   RgbaImage *image = new RgbaImage(m_width, m_height);
   
   image->setData(m_data);
   return image;
}

void RgbaImage::copyData(const Image *image) {
   ASSERT(image);
   
   if (image->isHDR()) {
      const HDRImage  *in = (const HDRImage  *)image;
      
      _setData(in->getData());
   } else {
      const RgbaImage *in = (const RgbaImage *)image;
      
      _setData(in->getData());
   }
}

}

