/**<!-------------------------------------------------------------------->
   @class  ProgressiveFilterValue
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Normalized value composed of a progressive sum of N contributions, 
   each of which has an associated weight
   <!-------------------------------------------------------------------->**/

#ifndef PROGRESSIVE_FILTER_VALUE_H_
#define PROGRESSIVE_FILTER_VALUE_H_

#include <utils/SpectralSampleSet.h>

namespace milton {

template <typename T = SpectralSampleSet>
struct MILTON_DLL_EXPORT ProgressiveFilterValue : public SSEAligned {
   T      numerator;
   real_t denominator;
   
   inline ProgressiveFilterValue()
      : numerator(), denominator(0)
   { }
   
   inline T getValue() const {
      if (denominator == 0)
         return T();
      
      return (numerator / denominator);
   }
   
   inline void addSample(const T &value, real_t weight) {
      numerator   += value * weight;
      denominator += weight;
   }
};

}

#endif // PROGRESSIVE_FILTER_VALUE_H_

