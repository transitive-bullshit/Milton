/**<!-------------------------------------------------------------------->
   @class  BoxFilter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D symmetric box filter (unweighted area filtering)
   <!-------------------------------------------------------------------->**/

#ifndef BOX_FILTER_H_
#define BOX_FILTER_H_

#include <filters/KernelFilter.h>

namespace milton {

class MILTON_DLL_EXPORT BoxFilter : public KernelFilter {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline BoxFilter(real_t support = 2)
         : KernelFilter(support)
      { }
      
      virtual ~BoxFilter()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the value of this filter function evaluated at the given 
       *    point
       */
      virtual real_t evaluate(const Vector2 &pt);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // BOX_FILTER_H_

