/**<!-------------------------------------------------------------------->
   @file   LanczosSincFilter.cpp
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

#include "LanczosSincFilter.h"

namespace milton {

void LanczosSincFilter::init() {
   KernelFilter::init();
   
   m_tau = getValue<real_t>("tau", m_tau);
}

real_t LanczosSincFilter::evaluate(const Vector2 &pt) {
   return _evaluate(pt[0]) * _evaluate(pt[1]);
}

real_t LanczosSincFilter::_evaluate(real_t x) const {
   x = fabs(x / m_support);
   
   if (x < EPSILON)
      return 1;
   
   if (x > 1)
      return 0;
   
   x *= M_PI;
   
   const real_t sinc    = sin(x * m_tau) / (x * m_tau);
   const real_t lanczos = sin(x) / x;
   
   return sinc * lanczos;
}

}

