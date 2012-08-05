/**<!-------------------------------------------------------------------->
   @class  GaussianFilter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D discrete, symmetric gaussian filter (separable like most of the 
   other filters, but we're not taking advantage of this to make the 
   filter framework cleaner and more cohesive), centered at the origin
   <!-------------------------------------------------------------------->**/

#ifndef GAUSSIAN_FILTER_H_
#define GAUSSIAN_FILTER_H_

#include <filters/KernelFilter.h>

namespace milton {

class MILTON_DLL_EXPORT GaussianFilter : public KernelFilter {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline GaussianFilter(real_t support = 2, real_t sigma = 1.0)
         : KernelFilter(support), m_sigma(sigma)
      { }
      
      virtual ~GaussianFilter()
      { }
      
      
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
      virtual real_t evaluate(const Vector2 &pt);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      /// @returns the sigma parameter of this gaussian distribution
      ///     (standard deviation)
      inline real_t getSigma() const {
         return m_sigma;
      }
      
      //@}-----------------------------------------------------------------
      
   protected:
      real_t m_sigma;
};

}

#endif // GAUSSIAN_FILTER_H_

