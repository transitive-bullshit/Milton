/**<!-------------------------------------------------------------------->
   @class  LanczosSincFilter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D symmetric Lanczos filter whose aim is to approximate a truncated 
   sinc (the ideal reconstruction filter) while minimizing the amount of 
   visible ringing resulting from the truncation
   
   @note
      LanczosSinc Filter taken from PBRT (Pharr, Humphreys)
      http://www.pbrt.org
   <!-------------------------------------------------------------------->**/

#ifndef LANCZOS_SINC_FILTER_H_
#define LANCZOS_SINC_FILTER_H_

#include <filters/KernelFilter.h>

namespace milton {

class MILTON_DLL_EXPORT LanczosSincFilter : public KernelFilter {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline LanczosSincFilter(real_t support = 2, real_t tau = 3.0)
         : KernelFilter(support), m_tau(tau)
      { }
      
      virtual ~LanczosSincFilter()
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
      
   protected:
      /// 1D LanczosSinc filter
      real_t _evaluate(real_t x) const;
      
   protected:
      real_t m_tau;
};

}

#endif // LANCZOS_SINC_FILTER_H_

