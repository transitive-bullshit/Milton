/**<!-------------------------------------------------------------------->
   @file   Log.inl
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Unified logging functionality for Milton debugging output
   <!-------------------------------------------------------------------->**/

#include "Log.h"

namespace milton {

// initialize static members of Log
std::ostream Log::s_null(NULL);

void Log::init() {
   if (_debug)
      _depth = _getDepth();
}

void Log::setDebug(bool debug) {
   if (debug != _debug) {
      _debug = debug;
      
      // TODO: figure out out to do this sheise cleanly..
      /*if (debug) {
         warning = DEBUG_STREAM_WARNING;
         error   = DEBUG_STREAM_ERROR;
         info    = DEBUG_STREAM_INFO;
      } else {
         warning = DEFAULT_STREAM_WARNING;
         error   = DEFAULT_STREAM_ERROR;
         info    = DEFAULT_STREAM_INFO;
      }*/
   }
}

}

