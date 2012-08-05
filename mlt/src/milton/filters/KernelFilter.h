/**<!-------------------------------------------------------------------->
   @class  KernelFilter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D discrete, symmetric filter which can be compactly / efficiently 
   stored / applied using a 2D kernel (array)
   <!-------------------------------------------------------------------->**/

#ifndef KERNEL_FILTER_H_
#define KERNEL_FILTER_H_

#include <filters/Filter.h>
#include <common/image/Rgba.h>

namespace milton {

class Image;

class MILTON_DLL_EXPORT KernelFilter : public Filter2D {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline KernelFilter(real_t support = 2)
         : Filter2D(support), 
           m_width(((unsigned)ceil(support)) | 1), 
           m_kernel(NULL), m_isNormalized(false)
      {
         // Note: cannot call _computeKernel in constructor because 
         // both it and evaluate (which the default implementation of 
         // _computeKernel calls) are virtual and the subclass' vtable 
         // is likely not initialized yet ==> bad consequences
      }
      
      virtual ~KernelFilter();
      
      
      //@}-----------------------------------------------------------------
      ///@name Static factory
      //@{-----------------------------------------------------------------
      
      static KernelFilter *create(const std::string &type, PropertyMap &p);
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the value of this filter function evaluated at the given 
       *    point
       */
      virtual real_t evaluate(const Vector2 &pt) = 0;
      
      /**
       * @brief
       *    Applies this filter to the given image, (performs discrete 
       * convolution with the given image and this 2D filter)
       * 
       * @note this involves allocating a temporary intermediate image
       */
      void apply(Image *image);
      
      /**
       * @brief
       *    Convolves this filter with the image at the given location and 
       * returns the result
       */
      RgbaHDR apply(Image *image, real_t x, real_t y);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      /// @returns the underlying 2D kernel
      inline const real_t *getKernel() const {
         return m_kernel;
      }
      
      /// @returns the width of the kernel (same as height since its symmetric)
      inline unsigned getWidth() const {
         return m_width;
      }
      
      /// @returns the number of real_ts contained in the kernel
      inline unsigned getSize() const {
         return m_width * m_width;
      }
      
      //@}-----------------------------------------------------------------
      
   protected:
      /// Allocates and initializes m_kernel
      virtual void _computeKernel();
      
   protected:
      unsigned m_width;
      real_t  *m_kernel;
      
      /// whether or not 'apply' should assume the kernel is normalized
      bool     m_isNormalized;
};

}

#endif // KERNEL_FILTER_H_

