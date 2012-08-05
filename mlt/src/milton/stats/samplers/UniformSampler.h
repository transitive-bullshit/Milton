/**<!-------------------------------------------------------------------->
   @class  UniformSampler
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a uniform distribution (either discrete or continuous), 
   parameterized by [min,max)
   <!-------------------------------------------------------------------->**/

#ifndef UNIFORM_SAMPLER_H_
#define UNIFORM_SAMPLER_H_

#include <stats/Sampler.h>

namespace milton {

template <typename T>
class MILTON_DLL_EXPORT UniformSampler : public Sampler {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit UniformSampler(T min = -std::numeric_limits<T>::max(), 
                                     T max = std::numeric_limits<T>::max())
         : Sampler(), 
           m_min(min), m_max(max)
      { }
      
      inline UniformSampler(const UniformSampler &copy)
         : Sampler(copy), 
           m_min(copy.m_min), m_max(copy.m_max)
      { }
      
      virtual ~UniformSampler()
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

      virtual void init() {
         Sampler::init();
         
         ASSERT(m_min <= m_max);
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors/Mutators
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the mean 'mu' of this normal random variable
       */
      inline T getMin() const {
         return m_min;
      }
      
      /**
       * @returns the variance 'sigma squared' of this normal random variable
       */
      inline T getMax() const {
         return m_max;
      }
      
      //@}-----------------------------------------------------------------
      
   protected:
      T m_min;
      T m_max;
};

}

#endif // UNIFORM_SAMPLER_H_

