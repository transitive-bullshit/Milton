/**<!-------------------------------------------------------------------->
   @class  JointContUniformSampler
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a uniform distribution on [min,max)^N ; N > 0, 
   which could be used, for example, for choosing a random UV coordinate
   (e.g, where N = 2, min = 0, max = 1)
   <!-------------------------------------------------------------------->**/

#ifndef JOINT_CONT_UNIFORM_SAMPLER_H_
#define JOINT_CONT_UNIFORM_SAMPLER_H_

#include "stats/samplers/ContUniformSampler.h"

namespace milton {

class MILTON_DLL_EXPORT JointContUniformSampler : public Sampler {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit JointContUniformSampler(unsigned min = 0, 
                                              unsigned max = 0, 
                                              unsigned dim = 2)
         : Sampler(), m_sampler(min, max), 
           m_dimension(dim)
      { }
      
      inline JointContUniformSampler(const JointContUniformSampler &copy)
         : Sampler(copy), m_sampler(copy.m_sampler), 
           m_dimension(copy.m_dimension)
      { }
      
      virtual ~JointContUniformSampler()
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
       * @returns the dimension of this distribution
       */
      inline unsigned getDimension() const {
         return m_dimension;
      }
      
      //@}-----------------------------------------------------------------
      
   protected:
      ContUniformSampler m_sampler;
      
      unsigned m_dimension;
};

}

#endif // JOINT_CONT_UNIFORM_SAMPLER_H_

