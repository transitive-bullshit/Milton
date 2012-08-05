/**<!-------------------------------------------------------------------->
   @file   HDRImage.inl
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2008
   
   @brief
     HDR image class supporting a 128- or 256-bit RgbaHDR pixel format
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_HDR_IMAGE_INL_
#define MILTON_HDR_IMAGE_INL_

namespace milton {

inline HDRImage::HDRImage(unsigned width, unsigned height)
   : Image(width, height), m_data(NULL)
{
   m_data = new RgbaHDR[m_size];
}

inline RgbaHDR *HDRImage::operator[](unsigned index) {
   ASSERT(index < m_height);
   
   return m_data + index * m_width;
}

inline const RgbaHDR *HDRImage::operator[](unsigned index) const {
   ASSERT(index < m_height);
   
   return m_data + index * m_width;
}

inline RgbaHDR *HDRImage::getData() {
   return m_data;
}

inline const RgbaHDR *HDRImage::getData() const {
   return m_data;
}

}

#endif // MILTON_HDR_IMAGE_INL_

