/**<!-------------------------------------------------------------------->
   @class  MultipleImportanceSampler
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

#ifndef MULTIPLE_IMPORTANCE_SAMPLER_H_
#define MULTIPLE_IMPORTANCE_SAMPLER_H_

#include <stats/WeightedEvent.h>
#include <utils/PropertyMap.h>

namespace milton {

class Sampler;

DECLARE_STL_TYPEDEF(std::vector<Sampler*>, SamplerList);
DECLARE_STL_TYPEDEF(std::vector<unsigned>, IntList);

class MILTON_DLL_EXPORT MultipleImportanceSampler : public PropertyMap {
   /// @note ni, n, and N use the same notation from Veach and Guibas' paper
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      /**
       * @param ni is the constant number of samples to take per sampler
       */
      inline MultipleImportanceSampler(const SamplerList &samplers, 
                                       unsigned ni = 1)
         : m_samplers(samplers), m_N(samplers.size()), m_n(m_N * ni), 
           m_ni(m_N, ni), m_computeWeightFunc(NULL)
      { }
      
      /**
       * @param ni holds the variable number of samples to take per sampler
       */
      inline MultipleImportanceSampler(const SamplerList &samplers, 
                                       const IntList &ni)
         : m_samplers(samplers), m_N(samplers.size()), m_n(0), m_ni(ni), 
           m_computeWeightFunc(NULL)
      {
         ASSERT(m_N == m_ni.size());
         
         FOREACH(IntListConstIter, m_ni, iter) {
            m_n += *iter;
         }
      }
      
      virtual ~MultipleImportanceSampler() 
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes the weight computation function based on this 
       * class' PropertyMap
       * 
       * @note defaults to balance heuristic
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Generates ni samples from each sampler pi in { p1,p1,...,pn }, 
       * along with sample weights, and stores the N weighted events in 
       * the out parameter @p results
       * 
       * @note 'results' is first cleared before adding any samples
       */
      virtual void sample(WeightedEventList &results);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      typedef void (MultipleImportanceSampler::*ComputeWeightFunc) 
         (WeightedEvent &event, unsigned i);
      
      // weighting heuristics
      virtual void _computeWeightBalance(WeightedEvent &event, unsigned i);
      virtual void _computeWeightCutoff (WeightedEvent &event, unsigned i);
      virtual void _computeWeightPower  (WeightedEvent &event, unsigned i);
      virtual void _computeWeightMaximum(WeightedEvent &event, unsigned i);
      
   protected:
      /// list of distributions from which to draw samples from
      SamplerList m_samplers;
      
      /// number of sampling distributions in m_samplers
      unsigned    m_N;
      
      /// total number of samples
      unsigned    m_n;
      
      /// sampler-by-sampler breakdown of number of samples
      IntList     m_ni;
      
      /// weight computation for individual samples (see init())
      ComputeWeightFunc m_computeWeightFunc;
      
      union {
         /// input to power heuristic
         real_t      m_beta;
         
         /// input to alpha heuristic
         real_t      m_alpha;
      };
};

}

#endif // MULTIPLE_IMPORTANCE_SAMPLER_H_

