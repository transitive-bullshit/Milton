/**<!-------------------------------------------------------------------->
   @class  Timer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Small utility timer
   
   @note
      Replaces the functionality of boost::timer which, at least on my primary 
   testing system (Fedora i686 Linux), yields drastically incorrect results...
   <!-------------------------------------------------------------------->**/

#ifndef TIMER_H_
#define TIMER_H_

#include <common/common.h>

#ifdef MILTON_ARCH_WINDOWS
#  include <boost/timer.hpp>
#else
#  include <sys/time.h>
#endif

namespace milton {

#ifdef MILTON_ARCH_WINDOWS

// note: for some reason, boost::timer was reporting incorrect times on my 
// primary development Linux platform, so as a workaround, I opted to use 
// gettimeofday on Posix platforms (Posix-specific function) which seems 
// to work fine. -- tfischer

struct MILTON_DLL_EXPORT Timer {
   inline Timer() 
      : m_timer()
   {
      reset();
   }
   
   inline void reset() {
      m_timer.restart();
   }
   
   /// @returns elapsed time measured in seconds since last reset
   inline double elapsed() const {
      return m_timer.elapsed();
   }
   
   private:
      boost::timer m_timer;
};

#else // Linux

struct MILTON_DLL_EXPORT Timer {
   inline Timer() {
      reset();
   }
   
   inline void reset() {
      gettimeofday(&m_startTime, 0);
   }
   
   /// @returns elapsed time measured in seconds since last reset
   inline double elapsed() const {
      timeval current_time;
      gettimeofday(&current_time, 0);
      
      current_time.tv_sec  -= m_startTime.tv_sec;
      current_time.tv_usec -= m_startTime.tv_usec;
      
      return current_time.tv_sec + 
         double(current_time.tv_usec) / CLOCKS_PER_SEC;
   }
   
   private:
      timeval m_startTime;
};

#endif

}

#endif // TIMER_H_

