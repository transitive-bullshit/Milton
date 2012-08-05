/**<!-------------------------------------------------------------------->
   @file   ExponentialSampler.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents an exponential distribution:
         X ~ Exp(lambda) ; lambda > 0
         f(x) = lambda * exp(-lambda * x) ; x >= 0
         E(X) = 1 / lambda
         Var(X) = 1 / (lambda ^ 2)
      
      Exponential distributions describe the times (waiting time) between 
   events in a Poisson process, i.e., a process in which events occur 
   continuously and independently at a constant average rate. An example 
   would be the average time you have to wait at a red light before the 
   light turns green.
   <!-------------------------------------------------------------------->**/

#include "ExponentialSampler.h"

namespace milton {

void ExponentialSampler::init() {
   Sampler::init();
   
   ASSERT(m_lambda > 0);
}

Event ExponentialSampler::sample() {
   return Event(m_sampler(), this);
}

real_t ExponentialSampler::getPd(const Event &event) {
   const real_t &x = event.getValue<real_t>();
   
   return m_lambda * exp(-m_lambda * x);
}

}

