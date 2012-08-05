/**<!-------------------------------------------------------------------->
   @class  RgbaImage
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2008
   
   @brief
     Image class supporting a standard 32-bit Rgba pixel format
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_RGBA_IMAGE_H_
#define MILTON_RGBA_IMAGE_H_

#include <common/image/Image.h>

class QImage;

namespace milton {

class MILTON_DLL_EXPORT RgbaImage : public Image {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   RgbaImage(unsigned width = 640, unsigned height = 480);
      explicit RgbaImage(const RgbaImage &copy);
      explicit RgbaImage(QImage &image);
      virtual ~RgbaImage();
      
      
      //@}-----------------------------------------------------------------
      ///@name Data Accessors / Mutators
      ///@note 
      ///   You can extract a particular pixel via [][].
      ///   For example:<br />
      ///      RgbaImage image;<br />
      ///      image[2][0] = Rgba32(255, 0, 0);<br />
      ///   This example sets the pixel in the third row, first column to red.
      //@{-----------------------------------------------------------------
      
      /**
       * @returns whether or not the underlying data for this image is stored 
       *    in a floating-point HDR format
       * 
       * @note that some operations will be more of less efficient when 
       *    performed on an HDR image versus a standard 32-bit Rgba image, 
       *    and knowing whether or not an image is in HDR format can 
       *    facilitate efficient usage (cutting down on the number of 
       *    implicit conversions between HDR and non-HDR pixel formats that 
       *    must be performed)
       * 
       * @note returns false for RgbaImage
       */
      virtual bool isHDR() const;
      
      /// @returns the underlying data in the given row index
      inline       Rgba32 *operator[](unsigned index);
      
      /// @returns the underlying data in the given row index
      inline const Rgba32 *operator[](unsigned index) const;
      
      /// @returns the underlying data in row major order
      inline Rgba32 *getData();
      
      /// @returns the underlying data in row major order
      inline const Rgba32 *getData() const;
      
      /**
       * @returns the luminance of the pixel at the given row and column which 
       *    is guaranteed to lie in [0, 1]
       */
      virtual real_t getLuminance(unsigned row, unsigned col) const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Mutators
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Sets the dimensions of this image, most likely allocating a new 
       * underlying buffer and erasing its previous contents
       */
      virtual void setSize(unsigned width, unsigned height);
      
      
      //@}-----------------------------------------------------------------
      ///@name Miscellaneous
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a copy of this Image and its underlying data
       */
      virtual Image *clone() const;
      
      /**
       * @brief
       *    Overwrites this image's data with the given image which is assumed 
       * to have the same dimensions as this image (and may have been created 
       * from this image by, for example, the clone function)
       */
      virtual void copyData(const Image *image);
      
      inline bool ownsData() const {
         return m_ownData;
      }
      
      inline void setOwnsData(bool ownsData) {
         m_ownData = ownsData;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual Rgba32  _getPixelRgba32 (unsigned row, unsigned col) const;
      virtual RgbaHDR _getPixelRgbaHDR(unsigned row, unsigned col) const;
      
      virtual void _setPixel(unsigned row, unsigned col, const Rgba32  &val);
      virtual void _setPixel(unsigned row, unsigned col, const RgbaHDR &val);
      
      virtual void _setData(const Rgba32  *data);
      virtual void _setData(const RgbaHDR *data);
      
   protected:
      Rgba32 *m_data;
      bool    m_ownData;
};

}

// include inline definitions
#include <common/image/RgbaImage.inl>

#endif // MILTON_RGBA_IMAGE_H_

