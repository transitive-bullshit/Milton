/**<!-------------------------------------------------------------------->
   @file   Event.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a single event which was sampled from a Sampler. The 
   internal value of this event is stored in a variant using boost::any, 
   where the concrete value of type T can be extracted using 
   Event<T>::getValue()
   
      This makes it esay to have all Samplers conform to the same sampling 
   interface, which facilitates the implementation of more abstract sampling 
   algorithms without respect to specific Sampler implementations.  One 
   example of this generic Sampling interface being useful is generic 
   multiple importance sampling (@see MultipleImportanceSampler)
   <!-------------------------------------------------------------------->**/

#include "Event.h"
#include "Sampler.h"

namespace milton {

real_t Event::getPd() {
   if (m_p < 0) {
      ASSERT(m_sampler);
      
      m_p = m_sampler->getPd(*this);
   }
   
   ASSERT(m_p >= 0);
   return m_p;
}

}

