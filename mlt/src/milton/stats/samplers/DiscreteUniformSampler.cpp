/**<!-------------------------------------------------------------------->
   @file   DiscreteUniformSampler.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a discrete uniform distribution
         X ~ U(min, max)
         p(x) = 1 / (max - min)
   <!-------------------------------------------------------------------->**/

#include "DiscreteUniformSampler.h"

namespace milton {

Event DiscreteUniformSampler::sample() {
   const int x = m_sampler();
   
   if (m_data)
      return Event(m_data[x], this);
   
   return Event(x, this);
}

real_t DiscreteUniformSampler::getPd(const Event &event) {
   return 1.0 / (m_max - m_min);
}

}

