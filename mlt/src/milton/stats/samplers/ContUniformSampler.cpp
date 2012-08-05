/**<!-------------------------------------------------------------------->
   @file   ContUniformSampler.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a continuous uniform distribution
         X ~ U(min, max)
         f(x) = 1 / (max - min)
   <!-------------------------------------------------------------------->**/

#include "ContUniformSampler.h"

namespace milton {

Event ContUniformSampler::sample() {
   return Event(m_sampler(), this);
}

real_t ContUniformSampler::getPd(const Event &event) {
   return 1.0 / (m_max - m_min);
}

}

