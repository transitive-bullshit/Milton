/**<!-------------------------------------------------------------------->
   @file   TriangleFilter.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D symmetric triangle filter (aka linear/tent filter)
   <!-------------------------------------------------------------------->**/

#include "TriangleFilter.h"

namespace milton {

real_t TriangleFilter::evaluate(const Vector2 &pt) {
   const real_t radius = m_support / 2.0;
   
   return MAX(0.0, 1.0 - pt[0] / radius) * MAX(0.0, 1.0 - pt[1] / radius);
}

}

