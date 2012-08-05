/**<!-------------------------------------------------------------------->
   @class  Log
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Unified logging functionality for Milton debugging output
   <!-------------------------------------------------------------------->**/

#ifndef LOG_H_
#define LOG_H_

#include <common/common.h>

#define DEBUG_STREAM_WARNING     (std::cerr)
#define DEBUG_STREAM_ERROR       (std::cerr)
#define DEBUG_STREAM_INFO        (std::cerr)

#ifdef DEBUG
#  define DEFAULT_STREAM_WARNING DEBUG_STREAM_WARNING
#  define DEFAULT_STREAM_ERROR   DEBUG_STREAM_ERROR
#  define DEFAULT_STREAM_INFO    DEBUG_STREAM_INFO
#  define DEFAULT_DEBUG          (true)
#else
#  define DEFAULT_STREAM_WARNING (Log::s_null)
#  define DEFAULT_STREAM_ERROR   (std::cerr)
#  define DEFAULT_STREAM_INFO    (Log::s_null)
#  define DEFAULT_DEBUG          (false)
#endif

namespace milton {

/**
 * @brief
 *    Unified Logging interface
 */
struct MILTON_DLL_EXPORT Log {
   // (default log is standard output when debug logging is enabled)
   inline Log(bool debug = false)
      : warning(debug ? DEBUG_STREAM_WARNING : DEFAULT_STREAM_WARNING), 
        error(debug   ? DEBUG_STREAM_ERROR   : DEFAULT_STREAM_ERROR), 
        info(debug    ? DEBUG_STREAM_INFO    : DEFAULT_STREAM_INFO), 
        _depth(0), _debug(debug ? true : DEFAULT_DEBUG)
   { }
   
   inline Log &operator=(Log &data) {
      _debug   = data._debug;
      
      return *this;
   }
   
   virtual void init();
   
   virtual void setDebug(bool debug);
   
   std::ostream &warning;
   std::ostream &error;
   std::ostream &info;
   
   protected:
      inline int _getDepth() const;
   
   protected:
      // stores depth in stack frames from last time 'init' was called
      int           _depth;
      
      // whether or not logging is enabled
      bool          _debug;
      
      // prints out to info stream
      friend inline std::ostream &operator<<(Log &data, const std::string &s);
      friend inline std::ostream &operator<<(Log &data, const char *s);
      
      static std::ostream s_null;
};

}

// include inline logging functions
#include <utils/Log.inl>

#endif // LOG_H_

