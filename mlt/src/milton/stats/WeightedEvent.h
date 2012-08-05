/**<!-------------------------------------------------------------------->
   @class  WeightedEvent
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents an event sampled by a Sampler that has been coupled with 
   an arbitrary real-valued weight in [0,1]
   <!-------------------------------------------------------------------->**/

#ifndef WEIGHTED_EVENT_H_
#define WEIGHTED_EVENT_H_

#include <stats/Event.h>

namespace milton {

struct MILTON_DLL_EXPORT WeightedEvent {
   Event  event;
   real_t weight;
   
   inline WeightedEvent(const Event &event_, real_t weight_ = 0)
      : event(event_), weight(weight_)
   {
      ASSERT(weight >= -EPSILON && weight <= 1 + EPSILON);
   }
};

DECLARE_STL_TYPEDEF(std::vector<WeightedEvent>, WeightedEventList);

}

#endif // WEIGHTED_EVENT_H_

