/**<!-------------------------------------------------------------------->
   @class  ExponentialSampler
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

#ifndef EXPONENTIAL_SAMPLER_H_
#define EXPONENTIAL_SAMPLER_H_

#include <stats/Sampler.h>

namespace milton {

class MILTON_DLL_EXPORT ExponentialSampler : public Sampler {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit ExponentialSampler(const real_t lambda = 1.0)
         : Sampler(), 
           m_sampler(Random::s_generator, 
                     Random::ExponentialDist(lambda)), 
           m_lambda(lambda)
      { }
      
      inline ExponentialSampler(const ExponentialSampler &copy)
         : Sampler(copy), 
           m_sampler(copy.m_sampler), 
           m_lambda(copy.m_lambda)
      { }
      
      virtual ~ExponentialSampler()
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
       * @returns the parameter lambda of this distribution, where 
       *    lambda is the expected 'rate'
       */
      inline real_t getLambda() const {
         return m_lambda;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      Random::BoostExponentialSampler m_sampler;
      
      real_t m_lambda;
};

}

#endif // EXPONENTIAL_SAMPLER_H_

