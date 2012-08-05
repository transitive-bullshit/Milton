/**<!-------------------------------------------------------------------->
   @class  DiscreteUniformSampler
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a discrete uniform distribution
         X ~ U(min, max)
         p(x) = 1 / (max - min)
   <!-------------------------------------------------------------------->**/

#ifndef DISCRETE_UNIFORM_SAMPLER_H_
#define DISCRETE_UNIFORM_SAMPLER_H_

#include <stats/samplers/UniformSampler.h>

namespace milton {

class MILTON_DLL_EXPORT DiscreteUniformSampler : public UniformSampler<int> {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit DiscreteUniformSampler(int min = 0, int max = 6)
         : UniformSampler<int>(min, max), 
           m_sampler(Random::s_generator, 
                     Random::DiscreteUniformDist(min, max)), 
           m_data(NULL)
      { }
      
      inline explicit DiscreteUniformSampler(const int *data, const unsigned n)
         : UniformSampler<int>(0, n), 
           m_sampler(Random::s_generator, 
                     Random::DiscreteUniformDist(0, n)), 
           m_data(data)
      { }
      
      inline DiscreteUniformSampler(const DiscreteUniformSampler &copy)
         : UniformSampler<int>(copy), 
           m_sampler(copy.m_sampler), 
           m_data(copy.m_data)
      { }
      
      virtual ~DiscreteUniformSampler()
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
      Random::BoostDiscreteUniformSampler m_sampler;
      
      const int                     *m_data;
};

}

#endif // DISCRETE_UNIFORM_SAMPLER_H_

