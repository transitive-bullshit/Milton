/**<!-------------------------------------------------------------------->
   @file   Sampler.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents an abstract random variable that can be sampled according 
   to some discrete/continuous probability distribution
   <!-------------------------------------------------------------------->**/

#include "Sampler.h"

namespace milton {

Event Sampler::sample(real_t &pdf) {
   const Event &x = sample();
   //ASSERT(m_sampleSpace && m_sampleSpace->contains(x));
   
   pdf = getPd(x);
   
   // pdf should be strictly positive because x was sampled, so we know it 
   // should have a non-zero probability density (and pdfs are always 
   // non-negative)
   ASSERT(pdf > 0);
   
   return x;
}

}

