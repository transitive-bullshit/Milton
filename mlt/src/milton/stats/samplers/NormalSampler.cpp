/**<!-------------------------------------------------------------------->
   @file   NormalSampler.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a normal distribution:
         X ~ N(u, sigma^2)
         f(x) = 1/(sqrt(2*pi*sigma^2)*exp(-(x-u)^2/(2*sigma^2)))
      The normal distribution (aka Gaussian) is parameterized by its mean 
   and variance and is referred to as the standard normal distribution when 
   when it has mean zero and variance one. The normal distribution is 
   probably the single most important distribution in all of probability 
   because of its ubiquity in describing natural phenomena and because of the 
   Central Limit Theorem, which says that the sum of a sufficiently large 
   number of iid random variables, each with finite mean and varirance, will 
   be approximately normally distributed.  This allows one to study almost 
   any distribution in terms of one, standard normal, distribution, simplifying
   many computations and proofs all over both applied and theoretical 
   statistics.
   <!-------------------------------------------------------------------->**/

#include "NormalSampler.h"

namespace milton {

void NormalSampler::init() {
   Sampler::init();
   
   ASSERT(m_variance >= 0);
}

Event NormalSampler::sample() {
   return Event(m_sampler(), this);
}

real_t NormalSampler::getPd(const Event &event) {
   const real_t &x = event.getValue<real_t>();
   const real_t xShift = (x - m_mean);
   
   return 1.0 / (sqrt(2 * M_PI * m_variance) * exp(xShift * xShift / (-2 * m_variance)));
}

}

