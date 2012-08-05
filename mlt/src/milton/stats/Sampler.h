/**<!-------------------------------------------------------------------->
   @class  Sampler
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents an abstract random variable that can be sampled according 
   to some discrete/continuous probability distribution
   <!-------------------------------------------------------------------->**/

#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <stats/Event.h>

namespace milton {

class MILTON_DLL_EXPORT Sampler {
   
   public:
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Should perform any initialization of this random variable which 
       * may be necessary to speed or prepare sampling
       * 
       * @note should be called before calling sample or getPd
       * @note default implementation is empty
       */
      virtual void init()
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
      virtual Event sample() = 0;
      
      /**
       * @brief
       *    Utility method to both sample this random variable and return 
       * the probability density with which that sample was chosen in 
       * the out variable pdf.
       * 
       * @returns a randomly chosen event x, sampled from this random 
       *    variable's sample space
       * @note consecutive calls to sample are expected to return 
       *    independent, identically distributed (IID) samples
       */
      virtual Event sample(real_t &pdf);
      
      /**
       * @returns the probability density with which the given event would be 
       *    sampled according to the underlying probability density function
       * @note the given event is assumed to lie within this random variable's
       *    sample space
       */
      virtual real_t getPd(const Event &event) = 0;
      
      
      //@}-----------------------------------------------------------------
};

}

#include <stats/Random.h>

#endif // SAMPLER_H_

