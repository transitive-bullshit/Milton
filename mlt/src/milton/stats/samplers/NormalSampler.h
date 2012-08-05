/**<!-------------------------------------------------------------------->
   @class  NormalSampler
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

#ifndef NORMAL_SAMPLER_H_
#define NORMAL_SAMPLER_H_

#include <stats/Sampler.h>

namespace milton {

class MILTON_DLL_EXPORT NormalSampler : public Sampler {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit NormalSampler(real_t mean = 0, real_t variance = 1)
         : Sampler(), 
           m_sampler(Random::s_generator, 
                     Random::NormalDist(mean, sqrt(variance))), 
           m_mean(mean), m_variance(variance)
      { }
      
      inline NormalSampler(const NormalSampler &copy)
         : Sampler(copy), 
           m_sampler(copy.m_sampler), 
           m_mean(copy.getMean()), m_variance(copy.getVariance())
      { }
      
      virtual ~NormalSampler()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Should perform any initialization of this random variable which 
       * may be necessary to speed or prepare sampling
       * 
       * @note should be called before calling sample or getPd
       * @note default implementation initialiazes this random variable's 
       *    sample space
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a randomly chosen event x, sampled from this random 
       *    variable's sample space
       * @note consecutive calls to sample are expected to return 
       *    independent, identically distributed (IID) samples
       */
      virtual Event sample();
      
      /**
       * @returns the probability density with which the given event would be 
       *    sampled according to the underlying probability density function
       * @note the given event is assumed to lie within this random variable's
       *    sample space
       */
      virtual real_t getPd(const Event &event);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors/Mutators
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the mean 'mu' of this normal random variable
       */
      inline real_t getMean() const {
         return m_mean;
      }
      
      /**
       * @returns the variance 'sigma squared' of this normal random variable
       */
      inline real_t getVariance() const {
         return m_variance;
      }
      
      /**
       * @returns the standard deviation 'sigma' of this normal random variable
       */
      inline real_t getStdDev() const {
         return m_sampler.distribution().sigma();
      }
      
      /**
       * @returns whether or not this normal distribution is approximately 
       *    standard with mean 0 and variance 1 (within EPSILON)
       */
      inline bool isStandardNormal() const {
         return (EQ(m_mean, create_real(0)) && EQ(m_variance, create_real(1)));
      }
      
      //@}-----------------------------------------------------------------
      
   protected:
      Random::BoostNormalSampler m_sampler;
      
      real_t m_mean;
      real_t m_variance;
};

}

#endif // NORMAL_SAMPLER_H_

