/**<!-------------------------------------------------------------------->
   @class  Image
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

#ifndef MILTON_IMAGE_H_
#define MILTON_IMAGE_H_

#include <common/image/Rgba.h>
#include <boost/shared_ptr.hpp>

namespace milton {

class MILTON_DLL_EXPORT Image {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   Image(unsigned width = 640, unsigned height = 480);
      virtual ~Image()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Saving / Loading
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Attempts to load an image from the given file, inferring the 
       * image file format by the filename's extension
       * 
       * @note If the actual file loaded is RgbaHDR and the requested image 
       *    format is RgbaImage, the loaded HDRImage image will be converted to 
       *    an RgbaImage before returning. Similarly, if the image was stored 
       *    in an Rgba32 format (PNG, JPEG, etc.), and the requested image 
       *    format is HDRImage, the loaded RgbaImage will be converted to an 
       *    HDRImage before returning.
       * 
       * @returns a new image of the desired type upon success or NULL upon 
       *    failur
       */
      static Image *load(const std::string &filename);
      
      /**
       * @brief
       *    Attempts to save this image out to the given file, inferring the 
       * desired file format by the filename's extension
       * 
       * @returns whether or not file was successfully saved
       */
      virtual bool save(const std::string &filename) const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Data Accessors / Mutators
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
       */
      virtual bool isHDR() const = 0;
      
      /**
       * @returns the pixel in the given row and column in the desired template
       *    format (either Rgba32 or RgbaHDR)
       * 
       * @note if this image is an HDR image and an Rgba32 is requested, an 
       *    implicit conversion is performed (and vice-versa if this image is 
       *    non-HDR and the caller requests an RgbaHDR)
       */
      template <typename T>
      inline T getPixel(unsigned row, unsigned col) const;
      
      /**
       * @brief
       *    Sets the pixel in the given row and column to the pixel value in 
       * the desired template format (either Rgba32 or RgbaHDR)
       * 
       * @note if this image is an HDR image and an Rgba32 value is given, an 
       *    implicit conversion is performed (and vice-versa if this image is 
       *    non-HDR and the caller gives an RgbaHDR)
       */
      template <typename T>
      inline void setPixel(unsigned row, unsigned col, const T &val);
      
      /**
       * @returns the luminance of the pixel at the given row and column which 
       *    is guaranteed to lie in [0, 1]
       */
      virtual real_t getLuminance(unsigned row, unsigned col) const = 0;
      
      
      //@}-----------------------------------------------------------------
      ///@name Size accessors
      //@{-----------------------------------------------------------------
      
      /// @returns the image width
      inline unsigned getWidth() const;
      
      /// @returns the image height
      inline unsigned getHeight() const;
      
      /// @returns width * height
      inline unsigned getSize() const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Large Mutators
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Sets the dimensions of this image, most likely allocating a new 
       * underlying buffer and erasing its previous contents
       */
      virtual void setSize(unsigned width, unsigned height) = 0;
      
      /**
       * @brief
       *    Sets the underlying data - data should be the size of the image
       * 
       * @note copies the given data into the underlying buffer, leaving 
       *    the given data unmodified
       * @note if this image is an HDR image and Rgba32 data is given, an 
       *    implicit conversion is performed (and vice-versa if this image is 
       *    non-HDR and the caller gives RgbaHDR data)
       */
      template <typename T>
      inline void setData(const T *data);
      
      /**
       * @brief
       *    Sets the underlying data and image dimensions, where the given data 
       * should be preallocated to the desired size of the image
       * 
       * @note copies the given data into a new underlying buffer, leaving 
       *    the given data unmodified
       * @note if this image is an HDR image and Rgba32 data is given, an 
       *    implicit conversion is performed (and vice-versa if this image is 
       *    non-HDR and the caller gives RgbaHDR data)
       */
      template <typename T>
      inline void setData(const T *data, unsigned width, unsigned height);
      
      
      //@}-----------------------------------------------------------------
      ///@name Miscellaneous
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a copy of this Image and its underlying data
       */
      virtual Image *clone() const = 0;
      
      /**
       * @brief
       *    Overwrites this image's data with the given image which is assumed 
       * to have the same dimensions as this image (and may have been created 
       * from this image by, for example, the clone function)
       */
      virtual void copyData(const Image *image) = 0;
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual Rgba32  _getPixelRgba32 (unsigned row, unsigned col) const = 0;
      virtual RgbaHDR _getPixelRgbaHDR(unsigned row, unsigned col) const = 0;
      
      virtual void _setPixel(unsigned row, unsigned col, const Rgba32  &val) = 0;
      virtual void _setPixel(unsigned row, unsigned col, const RgbaHDR &val) = 0;
      
      virtual void _setData(const Rgba32  *data) = 0;
      virtual void _setData(const RgbaHDR *data) = 0;
      
   protected:
      unsigned m_width;
      unsigned m_height;
      unsigned m_size;
};

typedef boost::shared_ptr<Image> ImagePtr;

}

// include inline definitions
#include <common/image/Image.inl>

#endif // MILTON_IMAGE_H_

