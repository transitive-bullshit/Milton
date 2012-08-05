/**<!-------------------------------------------------------------------->
   @file   HDRImage.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2008
   
   @brief
     HDR image class supporting a 128- or 256-bit RgbaHDR pixel format
   <!-------------------------------------------------------------------->**/

#include "HDRImage.h"
#include "RgbaImage.h"

namespace milton {

HDRImage::HDRImage(const HDRImage &copy) 
   : Image(copy.m_width, copy.m_height), m_data(NULL)
{
   m_data = new RgbaHDR[m_size];
   
   _setData(copy.getData());
}

HDRImage::~HDRImage() {
   safeDeleteArray(m_data);
}

bool HDRImage::isHDR() const {
   return true;
}

real_t HDRImage::getLuminance(unsigned row, unsigned col) const {
   ASSERT(row < m_height && col < m_width);
   
   return m_data[row * m_width + col].luminance();
}

Rgba32  HDRImage::_getPixelRgba32 (unsigned row, unsigned col) const {
   return static_cast<Rgba32>(m_data[row * m_width + col]);
}

RgbaHDR HDRImage::_getPixelRgbaHDR(unsigned row, unsigned col) const {
   return m_data[row * m_width + col];
}

void HDRImage::_setPixel(unsigned row, unsigned col, const Rgba32  &val) {
   m_data[row * m_width + col] = static_cast<RgbaHDR>(val);
}

void HDRImage::_setPixel(unsigned row, unsigned col, const RgbaHDR &val) {
   m_data[row * m_width + col] = val;
}

void HDRImage::setSize(unsigned width, unsigned height) {
   if (m_width != width || m_height != height) {
      m_size   = width * height;
      m_width  = width;
      m_height = height;
      
      RgbaHDR *data = m_data;
      m_data = new RgbaHDR[m_size];
      
      safeDeleteArray(data);
   }
}

void HDRImage::_setData(const Rgba32 *data) {
   ASSERT(m_data);
   ASSERT(data);
   
   for(unsigned i = m_height; i--;)
      for(unsigned j = m_width; j--;)
         _setPixel(i, j, data[i * m_width + j]);
}

void HDRImage::_setData(const RgbaHDR *data) {
   ASSERT(m_data);
   ASSERT(data);
   
   memcpy(m_data, data, sizeof(RgbaHDR) * m_size);
}

Image *HDRImage::clone() const {
   HDRImage *image = new HDRImage(m_width, m_height);
   
   image->setData(m_data);
   return image;
}

void HDRImage::copyData(const Image *image) {
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

