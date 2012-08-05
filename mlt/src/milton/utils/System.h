/**<!-------------------------------------------------------------------->
   @class  System
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2009

   @brief
      Static utility class providing information about the runtime environment 
   Milton is being run in; i.e., HW / OS information such as number of CPUs 
   available, OS name, etc.
   <!-------------------------------------------------------------------->**/

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <common/common.h>

namespace milton {
   
   class MILTON_DLL_EXPORT System {
      public:
         ///@name Utility methods
         //@{-----------------------------------------------------------------
         
         static unsigned getNoCPUs();
         
         //@}-----------------------------------------------------------------
         
      private:
         static unsigned s_noCPUs;
   };
   
}

#endif // SYSTEM_H_

