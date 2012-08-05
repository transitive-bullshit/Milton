/**<!-------------------------------------------------------------------->
   @file   Log.inl
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Unified logging functionality for Milton debugging output
   <!-------------------------------------------------------------------->**/

#ifndef LOG_INL_
#define LOG_INL_

#if defined(__STDC__) && defined(POSIX)
#  include <execinfo.h>
#else
//typedef USHORT (WINAPI *CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
//CaptureStackBackTraceType CaptureStackBackTrace = (CaptureStackBackTraceType)(GetProcAddress(LoadLibrary("kernel32.dll"), "RtlCaptureStackBackTrace"))
#endif

#define TAB_LENGTH_ (2)

namespace milton {

inline std::ostream &operator<<(Log &log, const std::string &s) {
   if (log._debug) {
      const int depth = MIN(255, MAX(0, log._getDepth() - log._depth));
      const std::string tab(depth * TAB_LENGTH_, ' ');
      
      // automatic indentation dependent on how deep the calling function 
      // is in the stack :)
      return (log.info << tab << s);
   }
   
   // restrict output with null ostream
   return Log::s_null;
}

inline std::ostream &operator<<(Log &log, const char *s) {
   if (log._debug) {
      const int curDepth = log._getDepth();
      const int depth = MIN(255, MAX(0, curDepth - log._depth));
      const std::string tab(depth * TAB_LENGTH_, ' ');
      
      // automatic indentation dependent on how deep the calling function 
      // is in the stack :)
      return (log.info << tab << s);
   }
   
   // restrict output with null ostream
   return Log::s_null;
}

inline int Log::_getDepth() const {
   size_t stack_depth;
   
#if defined(__STDC__) && defined(POSIX)
   const size_t max_depth = 256;
   void *stack_addrs[max_depth];
   
   stack_depth = backtrace(stack_addrs, max_depth) - 1;
#else
   // Random quote from Microsoft docs for Windows Server 2003 and Windows XP:
   //    The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
   const size_t max_depth = 62;
   void *stack_addrs[max_depth + 2];
   
   stack_depth = CaptureStackBackTrace(0, max_depth, stack_addrs, NULL) - 1;
#endif
   
   ASSERT(stack_depth >= 0);
   return stack_depth;
}

/*void print_trace(FILE *out, const char *file, int line) {
   const size_t max_depth = 100;
   size_t stack_depth;
   void *stack_addrs[max_depth];
   char **stack_strings;

   stack_depth = backtrace(stack_addrs, max_depth);
   stack_strings = backtrace_symbols(stack_addrs, stack_depth);

   fprintf(out, "Call stack from %s:%d:\n", file, line);

   for (size_t i = 1; i < stack_depth; i++) {
      fprintf(out, "    %s\n", stack_strings[i]);
   }
   free(stack_strings); // malloc()ed by backtrace_symbols
   fflush(out);
}*/

#undef TAB_LENGTH_

}

#endif // LOG_INL_

