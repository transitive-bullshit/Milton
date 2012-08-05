/**<!-------------------------------------------------------------------->
   @class  TriangleFilter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D symmetric triangle filter (aka linear/tent filter)
   <!-------------------------------------------------------------------->**/

#ifndef TRIANGLE_FILTER_H_
#define TRIANGLE_FILTER_H_

#include <filters/KernelFilter.h>

namespace milton {

class MILTON_DLL_EXPORT TriangleFilter : public KernelFilter {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline TriangleFilter(real_t support = 2)
         : KernelFilter(support)
      { }
      
      virtual ~TriangleFilter()
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

#endif // TRIANGLE_FILTER_H_

