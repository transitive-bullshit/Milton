/**<!-------------------------------------------------------------------->
   @class  System
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2009

   @brief
      Static utility class providing information about the runtime environment 
   Milton is being run in; i.e., HW / OS information such as number of CPUs 
   available, OS name, etc.
   
   @note
      Some HW detection code taken from xrdProofdProtocol by Gerardo Ganis 
   (copyright) 1995-2005.
   <!-------------------------------------------------------------------->**/

#include <System.h>

// System info on Solaris
#if (defined(SUNCC) || defined(__sun)) && !defined(__KCC)
#  define XPD__SUNCC
#  include <sys/systeminfo.h>
#  include <sys/filio.h>
#  include <sys/sockio.h>
#endif

// For process info
#if defined(__sun)
#  include <procfs.h>
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
#  include <sys/sysctl.h>
#endif

namespace milton {
   
   unsigned System::s_noCPUs = 0;
   
   unsigned System::getNoCPUs() {
      if (s_noCPUs <= 0) {
         unsigned noCPUs = 0;
         
#if defined(LINUX)                  // linux
         noCPUs = (unsigned) sysconf(_SC_NPROCESSORS_CONF);

#elif defined(MILTON_ARCH_WINDOWS)  // windows
         SYSTEM_INFO system_info;
         GetSystemInfo(&system_info);
         noCPUs = system_info.dwNumberOfProcessors;

#elif defined(__sun)                // solaris
         // run "psrinfo" in popen and count number of lines
         FILE *fp = popen("psrinfo", "r");
         
         if (fp != 0) {
            char line[2048] = { 0 };
            while (fgets(line, sizeof(line), fp))
               ++noCPUs;
            
            pclose(fp);
         }
         
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
         // run "sysctl -n hw.ncpu" in popen and decode the output
         FILE *fp = popen("sysctl -n hw.ncpu", "r");
         
         if (fp != 0) {
            char line[2048] = { 0 };
            while (fgets(line, sizeof(line), fp))
               noCPUs = GetLong(&line[0]);
            
            pclose(fp);
         }
         
#endif
         
         // cache the number of CPUs and guess 1 if we couldn't find the noCPUs
         s_noCPUs = (noCPUs > 0 ? noCPUs : 1);
      }
      
      ASSERT(s_noCPUs > 0);
      return s_noCPUs;
   }
   
}

