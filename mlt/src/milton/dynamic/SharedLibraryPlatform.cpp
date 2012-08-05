/*<!-------------------------------------------------------------------->
   @file   SharedLibraryPlatform.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Platform-independent interface for low-level operations on shared, 
   libraries, including platform-dependent implementations for POSIX-compliant 
   systems and Windows systems.
   <!-------------------------------------------------------------------->**/

#include "SharedLibraryPlatform.h"
#ifdef MILTON_ARCH_WINDOWS
#  include <iostream>
#  include <sstream>
using namespace std;
#else
#  include <dlfcn.h>
#endif

namespace milton {

#ifdef MILTON_ARCH_WINDOWS

static std::wstring s_s2ws(const std::string& s) {
   int slength = (int)s.length() + 1;
   
   int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
   wchar_t* buf = new wchar_t[len];
   
   MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
   
   std::wstring r(buf);
   safeDeleteArray(buf);
   return r;
}

SharedLibraryHandle SharedLibraryPlatformWindows::load(const std::string &name, 
                                                       int openFlags)
{
#ifdef UNICODE
   std::wstring stemp  = s_s2ws(name);
   LPCWSTR unicodeName = stemp.c_str();
#else
   LPCWSTR unicodeName = name.c_str();
#endif
   
   SharedLibraryHandle handle = LoadLibrary(unicodeName);
   
   if (!MILTON_IS_HANDLE_VALID(handle)) {
      std::stringstream s;
      s << "unknown error while attempting to load DLL '" << name << "'";
      m_lastError = s.str();
   }
   
   return handle;
}

bool SharedLibraryPlatformWindows::unload(SharedLibraryHandle handle) {
   ASSERT(MILTON_IS_HANDLE_VALID(handle));
   
   return (!!FreeLibrary(handle));
}

void *SharedLibraryPlatformWindows::getSymbol(SharedLibraryHandle handle, 
                                              const std::string &symbol)
{
   ASSERT(MILTON_IS_HANDLE_VALID(handle));
   LPCSTR unicodeSymbol = symbol.c_str();
   
   void *result = GetProcAddress(handle, unicodeSymbol);
   if (NULL == result) {
      std::stringstream s;
      s << "unknown error while attempting to load symbol '" << symbol << "'";
      m_lastError = s.str();
   }
   
   return result;
}

std::string SharedLibraryPlatformWindows::getLastError() {
   if (m_lastError == "") {
      LPVOID lpMsgBuf;
      DWORD dw = GetLastError(); 
      
      FormatMessage(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | 
         FORMAT_MESSAGE_FROM_SYSTEM |
         FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL,
         dw,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         (LPTSTR) &lpMsgBuf,
         0, NULL );

      // Display the error message and exit the process

      std::stringstream s;
      s  << "unknown error -- see milton::SharedLibraryPlatformWindows in sharedLibraryPlatform.cpp for details." << endl
         << "windows error code: " << GetLastError() << endl
         << (LPCTSTR)lpMsgBuf << endl;
      
      LocalFree(lpMsgBuf);
      return s.str();
   } else {
      return m_lastError;
   }
}

#else

SharedLibraryHandle SharedLibraryPlatformPosix::load(const std::string &name, 
                                                      int openFlags)
{
   if (openFlags == 0)
      openFlags = RTLD_LAZY;
   
   return dlopen(name.c_str(), openFlags);
}

bool SharedLibraryPlatformPosix::unload(SharedLibraryHandle handle) {
   ASSERT(MILTON_IS_HANDLE_VALID(handle));
   int ret = dlclose(handle);
   
   if (0 == ret) // success
      return true;
   
   return false;
}

void *SharedLibraryPlatformPosix::getSymbol(SharedLibraryHandle handle, 
                                            const std::string &symbol)
{
   ASSERT(MILTON_IS_HANDLE_VALID(handle));
   
   return dlsym(handle, symbol.c_str());
}

std::string SharedLibraryPlatformPosix::getLastError() {
   char *error = dlerror();
   
   if (error)
      return std::string(error);
   
   return std::string("");
}

#endif

}
