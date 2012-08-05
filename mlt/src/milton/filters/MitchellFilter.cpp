/**<!-------------------------------------------------------------------->
   @file   MitchellFilter.cpp
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

#include "MitchellFilter.h"

namespace milton {

void MitchellFilter::init() {
   KernelFilter::init();
   
   m_B = getValue<real_t>("B", m_B);
   m_C = getValue<real_t>("C", m_C);
}

real_t MitchellFilter::evaluate(const Vector2 &pt) {
   return _evaluate(pt[0]) * _evaluate(pt[1]);
}

real_t MitchellFilter::_evaluate(real_t x) const {
   x = ABS(2.0 * x / m_support);
   
   if (x > 1.0) {
      return 
         (((-m_B / 6.0 - m_C) * x + (m_B + 5.0 * m_C)) * x +
          (-2.0 * m_B - 8.0 * m_C)) * x + (4.0 / 3.0 * m_B + 4.0 * m_C);
   } else {
      return 
         x * x * ((2.0 - 1.5 * m_B - m_C) * x + (-3.0 + 2.0 * m_B + m_C))  +
         (1.0 - m_B / 3.0);
   }
}

}

