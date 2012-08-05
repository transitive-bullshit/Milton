/**<!-------------------------------------------------------------------->
   @file   KernelFilter.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D discrete, symmetric filter which can be compactly / efficiently 
   stored / applied using a 2D kernel (array)
   <!-------------------------------------------------------------------->**/

#include "KernelFilter.h"
#include <Image.h>
#include <filters.h>
using namespace std;

namespace milton {

KernelFilter *KernelFilter::create(const std::string &type, PropertyMap &p) {
   // Parse filter from PropertyMap (or defaults)
   const unsigned support = p.getValue<unsigned>("support", 2);
   
   if (type == "box") {
      return new BoxFilter(support);
   } else if (type == "triangle") {
      return new TriangleFilter(support);
   } else if (type == "gaussian") {
      const real_t sigma = p.getValue<real_t>("sigma", 1.0);
      
      return new GaussianFilter(support, sigma);
   } else if (type == "mitchell") {
      const real_t b = p.getValue<real_t>("B", 1.0 / 3.0);
      const real_t c = p.getValue<real_t>("C", 1.0 / 3.0);
      
      return new MitchellFilter(support, b, c);
   } else if (type == "lanczosSinc") {
      const real_t tau = p.getValue<real_t>("tau", 3.0);
      
      return new LanczosSincFilter(support, tau);
   } else {
      cerr << "encountered unexpected filter: " << type << endl;
      ASSERT(0 && "encountered unexpected filter");
      
      return NULL;
   }
}

KernelFilter::~KernelFilter() {
   safeDeleteArray(m_kernel);
}

void KernelFilter::init() {
   Filter2D::init();
   
   if (NULL == m_kernel) {
      _computeKernel();
      ASSERT(m_kernel);
   }
}

/*
// discretized filter evaluation
real_t KernelFilter::evaluate(const Vector2 &pt) {
   if (NULL == m_kernel) {
      _computeKernel();
      ASSERT(m_kernel);
   }
   
   const int x = floor(pt[0] + m_width / 2.0);
   const int y = floor(pt[1] + m_width / 2.0);
   
   if (x < 0 || x >= m_width || y < 0 || y >= m_width)
      return 0;
   
   return m_kernel[y * m_width + x];
}
*/

void KernelFilter::apply(Image *image) {
   ASSERT(image);
   
   if (NULL == m_kernel) {
      _computeKernel();
      ASSERT(m_kernel);
   }
   
   const int width  = image->getWidth();
   const int height = image->getHeight();
   const int size   = image->getSize();
   const int half   = (m_width >> 1);
   Image *temp = image->clone();
   
   // Apply the kernel to each pixel of the input image
   for(int i = height; i--;) {
      for(int j = width; j--;) {
         real_t *kernel = m_kernel;
         RgbaHDR rgb(0, 0, 0);
         real_t sum = 0;
         
         // Convolve kernel with current pixel of input image
         for(int y = -half; y <= half; ++y) {
            int row = CLAMP(i + y, 0, height - 1);
            
            for(int x = -half; x <= half; ++x) {
               int col = CLAMP(j + x, 0, width - 1);
               
               const RgbaHDR &d = image->getPixel<RgbaHDR>(row, col);
               real_t weight = *kernel++;
               sum   += weight;
               
               rgb.r += d.r * weight;
               rgb.g += d.g * weight;
               rgb.b += d.b * weight;
            }
         }
         
         temp->setPixel<RgbaHDR>(i, j, rgb / sum);
      }
   }
   
   image->copyData(temp);
   safeDelete(temp);
}

RgbaHDR KernelFilter::apply(Image *image, real_t x_, real_t y_) {
   ASSERT(image);
   
   if (NULL == m_kernel) {
      _computeKernel();
      ASSERT(m_kernel);
   }
   
   int x = (int) floor(x_), y = (int) floor(y_);
   const int width  = image->getWidth();
   const int height = image->getHeight();
   const int half   = (m_width >> 1);
   
   real_t *kernel = m_kernel;
   RgbaHDR rgb(0, 0, 0);
   real_t sum = 0;
   
   // Convolve kernel at a single location in input image
   for(int i = -half; i <= half; ++i) {
      int row = CLAMP(i + y, 0, height - 1);
      
      for(int j = -half; j <= half; ++j) {
         int col = CLAMP(j + x, 0, width - 1);
         
         const RgbaHDR &d = image->getPixel<RgbaHDR>(row, col);
         real_t weight = *kernel++;
         sum   += weight;
         
         rgb.r += d.r * weight;
         rgb.g += d.g * weight;
         rgb.b += d.b * weight;
      }
   }
   
   rgb /= sum;
   return rgb;
}

void KernelFilter::_computeKernel() {
   const real_t half = m_width / 2.0;
   ASSERT(m_width > 0);
   
   safeDeleteArray(m_kernel);
   m_kernel = new real_t[getSize()];
   
   for(unsigned i = m_width; i--;) {
      for(unsigned j = m_width; j--;) {
         const real_t val = evaluate(Vector2(i - half, j - half));
         
         m_kernel[i * m_width + j] = val;
      }
   }
}

}

