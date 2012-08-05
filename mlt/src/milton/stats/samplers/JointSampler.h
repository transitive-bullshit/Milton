/**<!-------------------------------------------------------------------->
   @class  JointSampler
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents an abstract joint random variable that can be sampled 
   according to several discrete/continuous probability distribution(s)
   <!-------------------------------------------------------------------->**/

#ifndef JOINT_SAMPLER_H_
#define JOINT_SAMPLER_H_

#include <stats/Sampler.h>

namespace milton {

DECLARE_STL_TYPEDEF(std::vector<Sampler*>, SamplerList);

class MILTON_DLL_EXPORT JointSampler : public Sampler {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit JointSampler(const SamplerList &randomVars)
         : Sampler(), m_samplers(randomVars)
      { }
      
      inline JointSampler(const JointSampler &copy)
         : Sampler(copy), m_samplers(copy.m_samplers)
      { }
      
      inline JointSampler()
         : Sampler()
      { }
      
      virtual ~JointSampler()
      { }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      SamplerList m_samplers;
};

}

#endif // JOINT_SAMPLER_H_

