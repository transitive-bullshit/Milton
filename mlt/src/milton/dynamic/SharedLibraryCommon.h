/*<!-------------------------------------------------------------------->
   @file   SharedLibraryCommon.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Definitions to ensure shared library support will be cross-platform
   <!-------------------------------------------------------------------->**/

#ifndef SHARED_LIBRARY_COMMON_H_
#define SHARED_LIBRARY_COMMON_H_

#include <common/common.h>

namespace milton {

#ifdef   MILTON_ARCH_WINDOWS
   typedef HINSTANCE SharedLibraryHandle;
   
#define MILTON_IS_HANDLE_VALID(handle) \
   (handle != 0)
#else // MILTON_ARCH_UNIX
   typedef void*     SharedLibraryHandle;
   
#define MILTON_IS_HANDLE_VALID(handle) \
   (handle != NULL)
#endif

}

#endif // SHARED_LIBRARY_COMMON_H_

