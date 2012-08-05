/**<!-------------------------------------------------------------------->
  @file   Rgba.h
  @author Travis Fischer (fisch0920@gmail.com)
  @date   Fall 2008

  @brief
     32-bit and HDR (128- or 256-bit floating point) Rgba structs
  <!-------------------------------------------------------------------->**/

#ifndef MILTON_RGBA_H_
#define MILTON_RGBA_H_

#include <common/math/Vector.h>

namespace milton {

struct Rgba32;
struct RgbaHDR;

/**
 * @brief
 *    Standard 32-bit RGBA pixel with red, green, blue, and alpha channels, 
 * all with integer values in the range [0, 255]
 */
struct MILTON_DLL_EXPORT Rgba32 {
   union {
      struct { unsigned char b, g, r, a; };
      
      unsigned char data[4];
   };
   
   inline explicit Rgba32(unsigned char r_ = 0, unsigned char g_ = 0, 
                          unsigned char b_ = 0, unsigned char a_ = 255);
   
   static Rgba32 fromVector(const Vector3 &v) {
      Rgba32 rgba;
      
      rgba.b = (unsigned char) CLAMP(255 * v[2], 0, 255);
      rgba.g = (unsigned char) CLAMP(255 * v[1], 0, 255);
      rgba.r = (unsigned char) CLAMP(255 * v[0], 0, 255);
      
      return rgba;
   }
   
   inline unsigned char  operator[](const unsigned i) const;
   inline unsigned char &operator[](const unsigned i);
   
   /// @returns this Rgba as a Vector3 (implicit conversion) for 
   ///    convenience purposes
   inline operator Vector3() const;
   
   /// @returns this Rgba32 as a RgbaHDR
   inline operator RgbaHDR() const;
   
   /// @returns the luminance of this Rgba
   inline real_t luminance() const;
};

/**
 * @brief
 *    Standard 128- or 256-bit RGBA pixel with floating-point red, green, 
 * blue, and alpha channels
 */
struct MILTON_DLL_EXPORT RgbaHDR {
   union {
      struct { real_t r, g, b, a; };
      
      real_t data[4];
   };
   
   inline explicit RgbaHDR(real_t r_ = 0, real_t g_ = 0, 
                           real_t b_ = 0, real_t a_ = 1);
   
   static RgbaHDR fromVector(const Vector3 &v) {
      return RgbaHDR(v[0], v[1], v[2]);
   }
   
   inline real_t  operator[](const unsigned i) const;
   inline real_t &operator[](const unsigned i);
   
   /// @returns this Rgba as a Vector3 (implicit conversion) for 
   /// @returns this RgbaHDR as a Vector3 (implicit conversion) for 
   ///    convenience purposes
   inline operator Vector3() const;
   
   /// @returns this RgbaHDR as a 32-bit Rgba32
   inline operator Rgba32() const;
   
   /// @returns the luminance of this Rgba
   inline real_t luminance() const;
   
   /// @returns { r * s, g * s, b * s, a }
   inline RgbaHDR operator*(real_t scalar) const;
   
   /// @returns { r / s, g / s, b / s, a }
   inline RgbaHDR operator/(real_t scalar) const;
   
   /// sets this rgba to { r * s, g * s, b * s, a }
   inline RgbaHDR &operator*=(real_t scalar);
   
   /// sets this rgba to { r / s, g / s, b / s, a }
   inline RgbaHDR &operator/=(real_t scalar);
};

}

// include inline definitions
#include <common/image/Rgba.inl>

#endif // MILTON_RGBA_H_

