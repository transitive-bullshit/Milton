/**<!-------------------------------------------------------------------->
   @file   MultipleImportanceSampler.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      "Optimally" combines samples taken from multiple sampling distributions 
   with respect to a function, 'f', whose value we are trying to integate 
   over a given domain,'D'.  D is assumed to be the union of the domains of 
   the individual underlying distributions.
      The goal of importance sampling is to reduce variance while 
   approximating the integral of f over D by drawing random samples across D 
   according to some probability density function p which is proportional to 
   f.  By drawing more samples from the regions where f is large (concentrating
   our sampling effort on the "important" regions of the domain), and similarly
   drawing less samples from the regions where f is small (focusing less effort
   in the "unimportant" regions of the domain), the variance of our estimate 
   overall is reduced, as long as we compensate for our uneven sampling rate 
   by weighting each sample by the inverse probability with which it was 
   sampled.
      Think of importance sampling this way:  if we have only one shot at 
   sampling f (only one sample because of very limited 'resources'), we would 
   like to concentrate that one sample in the region of the domain that will 
   contribute the most to the value of the integral we are ultimately trying 
   to approximate.  By biasing our sampling technique to weigh "important", 
   "large" regions of the domain with respect to f, we get more bang for our
   buck and correspondingly end up with a much lower variance estimator. If 
   we were to instead naively sample uniformly across the domain, D, there's 
   a good chance that our one, precious sample would be wasted by sampling 
   a location, x, that is unimportant, where f(x) is relatively small.
      A perfect estimator would take samples from a distribution p, across D, 
   whose probabilities were distributed directly proportional to f.  This is, 
   however, unreasonable, since if we knew the exact values of f across D, 
   we wouldn't have to approximate the integral of f over D in the first place.
   We can, however, still greatly reduce variance by using knowledge about 
   f to guide our sampling process and instead, sample according to either 
   an approximation of f, or possibly according to some factorization of f 
   into separate functions f1, f2, etc, some of which may be easier to draw 
   samples from.  As long as our sampling process attempts to draw samples 
   from areas in D where f is "known" to be large and avoids regions in D 
   where f is "known" to be small, our sampler will be more efficient and 
   our overall variance will be reduced.
   
      Multiple importance sampling generalizes this one step further. Say 
   we have n different sampling distributions p1,p2,...,pn, and our function 
   f, may be separated or factored into several functions, f1,f2,...fm. By 
   designing different sampling distributions to focus on one or more of the 
   simpler functions, fi, we can combine samples from the different sampling 
   distributions, each of which may be better at estimating some important 
   component (subfunction) of f, we can gain a much better final sampling 
   distribution, p, which captures all of the relevant aspects of f in an 
   intuitive, modularized manner (where each pi can be tuned towards 
   capturing a different fi).  How to go about combining samples from n 
   distributions, all or some of which may be good or bad estimators of f, 
   is a very difficult problem in this generic of a setting.
      Veach and Guibas (1995) showed several provably good ways of optimally 
   combining sampling techniques for Monte Carlo integration.  This class 
   implements their multiple importance sampling model, including several 
   heuristics they gave which attempt to combine samples in slightly 
   different ways that may work better in one situation versus another.
   These include the balance, cutoff, power, and maximum heuristics.
   
   @note for more information and theoretical details, see
      Veach and Guibas. Optimally Combining Sampling Techniques for Monte 
      Carlo Rendering. In Proceedings of the 22nd Annual Conference on 
      Computer Graphics and interactive Techniques S.G. Mair and R. Cook, 
      Eds. SIGGRAPH '95. ACM, New York, NY, 419-428.
   
   @see http://www-graphics.stanford.edu/papers/combine/
   <!-------------------------------------------------------------------->**/

#include "MultipleImportanceSampler.h"
#include "Sampler.h"

namespace milton {

void MultipleImportanceSampler::init() {
   const std::string &weightFunc = getValue<std::string>("weightFunc", 
                                                         "balance");
   
   if (weightFunc == "balance") {
      m_computeWeightFunc = &MultipleImportanceSampler::_computeWeightBalance;
   } else if (weightFunc == "power") {
      m_computeWeightFunc = &MultipleImportanceSampler::_computeWeightPower;
      
      m_beta  = getValue<real_t>("beta",  2.0);
   } else if (weightFunc == "cutoff") {
      m_computeWeightFunc = &MultipleImportanceSampler::_computeWeightCutoff;
      
      m_alpha = getValue<real_t>("alpha", 0.1);
   } else {
      ASSERT(weightFunc == "maximum");
      m_computeWeightFunc = &MultipleImportanceSampler::_computeWeightMaximum;
   }
}

void MultipleImportanceSampler::sample(WeightedEventList &results) {
   ASSERT(m_computeWeightFunc != NULL);
   results.clear();
   
   for(unsigned i = m_n; i--;) {
      Sampler *sampler  = m_samplers[i];
      const unsigned ni = m_ni[i];
      
      for(unsigned j = ni; j--;) {
         results.push_back(WeightedEvent(sampler->sample()));
         ((this)->*(m_computeWeightFunc))(results.back(), i);
      }
   }
}

void MultipleImportanceSampler::_computeWeightBalance(WeightedEvent &e, 
                                                      unsigned i)
{
   ASSERT(i < m_n);
   real_t num = 0, den = 0;
   
   // TODO: this is incorrect; m_ni[j] doesn't exist cause m_n goes up too 
   // high!!!
   NYI();
   for(unsigned j = m_n; j--;) {
      if (i == j)
         den += (num = ((m_ni[j] / real_t(m_N)) * e.event.getPd()));
      else 
         den += (m_ni[j] / real_t(m_N)) * m_samplers[i]->getPd(e.event);
   }
   
   ASSERT(den > 0);
   e.weight = num / den;
}

void MultipleImportanceSampler::_computeWeightCutoff(WeightedEvent &e, 
                                                     unsigned i)
{
   ASSERT(i < m_n);
   const real_t pi = e.event.getPd();
   real_t pMax = pi;
   
   for(unsigned j = m_n; j--;) {
      const real_t p = (i == j ? pi : m_samplers[i]->getPd(e.event));
      pMax = MAX(p, pMax);
   }
   
   pMax *= m_alpha;
   e.weight = 0;
   
   if (pi >= pMax) {
      for(unsigned j = m_n; j--;) {
         const real_t p = (i == j ? pi : m_samplers[i]->getPd(e.event));
         
         if (p >= pMax)
            e.weight += p;
      }
      
      e.weight = pi / e.weight;
   } // else set weight to zero (disregards samples below a cutoff threshold)
}

void MultipleImportanceSampler::_computeWeightPower(WeightedEvent &e, 
                                                    unsigned i)
{
   ASSERT(i < m_n);
   const real_t pi = pow(e.event.getPd(), m_beta);
   
   e.weight = 0;
   for(unsigned j = m_n; j--;) {
      const real_t p = (i == j ? pi : pow(m_samplers[i]->getPd(e.event), m_beta));
      
      e.weight += p;
   }
   
   e.weight = pi / e.weight;
}

void MultipleImportanceSampler::_computeWeightMaximum(WeightedEvent &e, 
                                                      unsigned i)
{
   ASSERT(i < m_n);
   const real_t pi = e.event.getPd();
   real_t pMax = pi;
   
   for(unsigned j = m_n; j--;) {
      const real_t p = (i == j ? pi : m_samplers[i]->getPd(e.event));
      pMax = MAX(p, pMax);
   }
   
   e.weight = (pi == pMax);
}

}

