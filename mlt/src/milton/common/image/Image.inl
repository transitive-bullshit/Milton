/**<!-------------------------------------------------------------------->
   @file   Image.inl
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
   Default/built-in image formats supported by Qt:
   
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

#ifndef MILTON_IMAGE_INL_
#define MILTON_IMAGE_INL_

namespace milton {

inline Image::Image(unsigned width, unsigned height)
   : m_width(width), m_height(height), m_size(width * height)
{ }

template <typename T>
inline T Image::getPixel(unsigned row, unsigned col) const {
   ASSERT(row < m_height && col < m_width);
   
   if (sizeof(T) == sizeof(Rgba32))
      return _getPixelRgba32(row, col);
   
   return _getPixelRgbaHDR(row, col);
}

template <typename T>
inline void Image::setPixel(unsigned row, unsigned col, const T &val) {
   ASSERT(row < m_height && col < m_width);
   
   _setPixel(row, col, val);
}

inline unsigned Image::getWidth() const {
   return m_width;
}

inline unsigned Image::getHeight() const {
   return m_height;
}

inline unsigned Image::getSize() const {
   return m_size;
}

template <typename T>
inline void Image::setData(const T *data) {
   _setData(data);
}

template <typename T>
inline void Image::setData(const T *data, unsigned width, unsigned height) {
   setSize(width, height);
   
   _setData(data);
}

}

#endif // MILTON_IMAGE_INL_

