/**<!-------------------------------------------------------------------->
   @class  JointEvent
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a single event which was sampled from a JointSampler. The 
   internal value of this event is stored in a variant using boost::any, 
   where the concrete value of type T can be extracted using 
   Event<T>::getValue()
   <!-------------------------------------------------------------------->**/

#ifndef JOINT_EVENT_H_
#define JOINT_EVENT_H_

#include <stats/Event.h>

namespace milton {

class JointSampler;

DECLARE_STL_TYPEDEF(std::vector<Event>, EventList);

class MILTON_DLL_EXPORT JointEvent : public Event {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit JointEvent(const EventList &eventList, 
                                 Sampler *randomVar = NULL)
         : Event(eventList, randomVar)
      { }
      
      inline JointEvent()
         : Event()
      { }
      
      virtual ~JointEvent()
      { }
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // JOINT_EVENT_H_

