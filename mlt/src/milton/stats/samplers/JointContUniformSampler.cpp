/**<!-------------------------------------------------------------------->
   @file   JointContUniformSampler.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a uniform distribution on [min,max)^N ; N > 0, 
   which could be used, for example, for choosing a random UV coordinate
   (e.g, where N = 2, min = 0, max = 1)
   <!-------------------------------------------------------------------->**/

#include "JointContUniformSampler.h"

namespace milton {

void JointContUniformSampler::init() {
   Sampler::init();
   m_sampler.init();
   
   ASSERT(m_dimension > 0);
}

Event JointContUniformSampler::sample() {
   std::vector<real_t> sample;
   
   for(unsigned i = m_dimension; i--;)
      sample.push_back(m_sampler.sample().getValue<real_t>());
   
   return Event(sample, this);
}

real_t JointContUniformSampler::getPd(const Event &event) {
   const real_t den = pow((m_sampler.getMax() - m_sampler.getMin()), 
                          static_cast<real_t>(m_dimension));
   
   return 1.0 / den;
}

}

