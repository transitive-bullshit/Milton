/**<!-------------------------------------------------------------------->
   @file   BoxFilter.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D symmetric box filter (unweighted area filtering)
   <!-------------------------------------------------------------------->**/

#include "BoxFilter.h"

namespace milton {

real_t BoxFilter::evaluate(const Vector2 &pt) {
   return (2.0 * sqrt(pt[0] * pt[0] + pt[1] * pt[1]) < m_support);
}

}

