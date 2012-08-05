/**<!-------------------------------------------------------------------->
  @file   Rgba.inl
  @author Travis Fischer (fisch0920@gmail.com)
  @date   Fall 2008

  @brief
     32-bit and 128-bit Rgba structs
  <!-------------------------------------------------------------------->**/

#ifndef MILTON_RGBA_INL_
#define MILTON_RGBA_INL_

namespace milton {

inline Rgba32::Rgba32(unsigned char r_, unsigned char g_, 
                      unsigned char b_, unsigned char a_)
   : b(b_), g(g_), r(r_), a(a_)
{ }

inline unsigned char  Rgba32::operator[](const unsigned i) const {
   ASSERT(i < 4);
   
   return data[i];
}

inline unsigned char &Rgba32::operator[](const unsigned i) {
   ASSERT(i < 4);
   
   return data[i];
}

inline Rgba32::operator Vector3() const {
   return Vector3(CLAMP(r / 255.0, 0, 1), 
                  CLAMP(g / 255.0, 0, 1),  
                  CLAMP(b / 255.0, 0, 1));
}

inline Rgba32::operator RgbaHDR() const {
   return RgbaHDR::fromVector(*this);
}

inline real_t Rgba32::luminance() const {
   //return (0.299 * r + 0.587 * g + 0.11 * b) / 255;
   return (0.2126 * r + 0.7152 * g + 0.0722 * b) / 255;
}


inline RgbaHDR::RgbaHDR(real_t r_, real_t g_, 
                        real_t b_, real_t a_)
   : r(r_), g(g_), b(b_), a(a_)
{ }

inline real_t  RgbaHDR::operator[](const unsigned i) const {
   ASSERT(i < 4);
   
   return data[i];
}

inline real_t &RgbaHDR::operator[](const unsigned i) {
   ASSERT(i < 4);
   
   return data[i];
}

inline RgbaHDR::operator Vector3() const {
   return Vector3(r, g, b);
}

inline RgbaHDR::operator Rgba32() const {
   return Rgba32::fromVector(*this);
}

inline real_t RgbaHDR::luminance() const {
   //return (0.299 * r + 0.587 * g + 0.11 * b);
   return (0.2126 * r + 0.7152 * g + 0.0722 * b);
}

inline RgbaHDR RgbaHDR::operator*(real_t scalar) const {
   return RgbaHDR(r * scalar, g * scalar, b * scalar, a);
}

inline RgbaHDR RgbaHDR::operator/(real_t scalar) const {
   ASSERT(scalar != 0);
   
   return RgbaHDR(r / scalar, g / scalar, b / scalar, a);
}

inline RgbaHDR &RgbaHDR::operator*=(real_t scalar) {
   r *= scalar;
   g *= scalar;
   b *= scalar;
   
   return *this;
}

inline RgbaHDR &RgbaHDR::operator/=(real_t scalar) {
   ASSERT(scalar != 0);
   
   r /= scalar;
   g /= scalar;
   b /= scalar;
   
   return *this;
}

}

#endif // MILTON_RGBA_INL_

