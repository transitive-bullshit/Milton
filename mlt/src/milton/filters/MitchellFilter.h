/**<!-------------------------------------------------------------------->
   @class  MitchellFilter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D symmetric Mitchell filter which is parameterized to tradeoff between 
   'ringing' and 'blurring' that other filters tend towards in difficult 
   reconstruction cases
   
   @note
      Mitchell Filter taken from PBRT (Pharr, Humphreys)
      http://www.pbrt.org
   <!-------------------------------------------------------------------->**/

#ifndef MITCHELL_FILTER_H_
#define MITCHELL_FILTER_H_

#include <filters/KernelFilter.h>

namespace milton {

class MILTON_DLL_EXPORT MitchellFilter : public KernelFilter {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MitchellFilter(real_t support = 2, 
                            real_t b = 1.0 / 3.0, 
                            real_t c = 1.0 / 3.0)
         : KernelFilter(support), m_B(b), m_C(c)
      { }
      
      virtual ~MitchellFilter()
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
      /// 1D Mitchell filter
      real_t _evaluate(real_t x) const;
      
   protected:
      real_t m_B;
      real_t m_C;
};

}

#endif // MITCHELL_FILTER_H_

