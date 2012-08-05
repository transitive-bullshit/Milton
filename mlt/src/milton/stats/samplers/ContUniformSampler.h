/**<!-------------------------------------------------------------------->
   @class  ContUniformSampler
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a continuous uniform distribution
         X ~ U(min, max)
         f(x) = 1 / (max - min)
   <!-------------------------------------------------------------------->**/

#ifndef CONT_UNIFORM_SAMPLER_H_
#define CONT_UNIFORM_SAMPLER_H_

#include <stats/samplers/UniformSampler.h>

namespace milton {

class MILTON_DLL_EXPORT ContUniformSampler : public UniformSampler<real_t> {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit ContUniformSampler(real_t min = 0, real_t max = 1)
         : UniformSampler<real_t>(min, max), 
           m_sampler(Random::s_generator, 
                     Random::ContUniformDist(min, max))
      { }
      
      inline ContUniformSampler(const ContUniformSampler &copy)
         : UniformSampler<real_t>(copy), 
           m_sampler(copy.m_sampler)
      { }
      
      virtual ~ContUniformSampler()
      { }
      
      
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
      
   protected:
      Random::BoostContUniformSampler m_sampler;
};

}

#endif // CONT_UNIFORM_SAMPLER_H_

