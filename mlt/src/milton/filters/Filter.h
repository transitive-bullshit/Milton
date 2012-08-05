/**<!-------------------------------------------------------------------->
   @class  Filter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Templated representation of a symmetric N-dimensional filter function 
   in T^N centered at zero with known max support (where the function is zero 
   outside of that radius of that support; ie, support is equivalent to 
   diameter)
   <!-------------------------------------------------------------------->**/

#ifndef FILTER_H_
#define FILTER_H_

#include <utils/PropertyMap.h>
#include <common/math/Vector.h>

namespace milton {

template <unsigned N = 2, typename T = real_t>
class MILTON_DLL_EXPORT Filter : public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   Filter(real_t support = 2)
         : m_support(support)
      { }
      
      virtual ~Filter()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization which may be necessary before filter  
       * evaluation
       * 
       * @note
       *    Default implementation initializes the filter support based on 
       * its PropertyMap
       */
      virtual void init() {
         m_support = getValue<real_t>("support", m_support);
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the value of this filter function evaluated at the given 
       *    point
       */
      virtual T evaluate(const Vector2 &pt) = 0;
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      /// @returns the size of the support of this filter (symmetric)
      inline real_t getSupport() const {
         return m_support;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      real_t m_support;
};

typedef Filter<2, real_t> Filter2D;
typedef Filter<1, real_t> Filter1D;

}

#endif // FILTER_H_

