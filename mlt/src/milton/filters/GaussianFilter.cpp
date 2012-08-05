/**<!-------------------------------------------------------------------->
   @file   GaussianFilter.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      2D discrete, symmetric gaussian filter (separable like most of the 
   other filters, but we're not taking advantage of this to make the 
   filter framework cleaner and more cohesive), centered at the origin
   <!-------------------------------------------------------------------->**/

#include "GaussianFilter.h"

namespace milton {

void GaussianFilter::init() {
   KernelFilter::init();
   
   m_sigma = getValue<real_t>("sigma", m_sigma);
}

real_t GaussianFilter::evaluate(const Vector2 &pt) {
   ASSERT(m_sigma > 0); // note assuming sigma positive when it suffices to 
                        // assume sigma is non-negative ; shouldn't matter
                        // but it prevents a special case for sigma at zero
   
   const real_t coeff  = (-0.5) / (m_sigma * m_sigma);
   const real_t weight = coeff * (pt[0] * pt[0] + pt[1] * pt[1]);
   
   return MAX(0.0, (1.0 / (sqrt(2.0 * M_PI) * m_sigma)) * exp(weight));
}

}

