/**<!-------------------------------------------------------------------->
   @file   RgbaImage.inl
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2008
   
   @brief
     Image class supporting a standard 32-bit Rgba pixel format
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_RGBA_IMAGE_INL_
#define MILTON_RGBA_IMAGE_INL_

namespace milton {

inline RgbaImage::RgbaImage(unsigned width, unsigned height)
   : Image(width, height), m_data(NULL), m_ownData(true)
{
   m_data = new Rgba32[m_size];
}

inline Rgba32 *RgbaImage::operator[](unsigned index) {
   ASSERT(index < m_height);
   
   return m_data + index * m_width;
}

inline const Rgba32 *RgbaImage::operator[](unsigned index) const {
   ASSERT(index < m_height);
   
   return m_data + index * m_width;
}

inline Rgba32 *RgbaImage::getData() {
   return m_data;
}

inline const Rgba32 *RgbaImage::getData() const {
   return m_data;
}

}

#endif // MILTON_RGBA_IMAGE_INL_

