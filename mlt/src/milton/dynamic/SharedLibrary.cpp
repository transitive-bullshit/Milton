/*<!-------------------------------------------------------------------->
   @file   SharedLibrary.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Platform-independent utility wrapper around a dynamically loaded shared 
   library.
   <!-------------------------------------------------------------------->**/

#include "SharedLibrary.h"
#include "SharedLibraryPlatform.h"

namespace milton {

SharedLibrary::SharedLibrary(const std::string &name, 
                             SharedLibraryHandle handle, 
                             SharedLibraryPlatform *platform)
   : m_name(name), m_handle(handle), m_platform(platform)
{
   ASSERT(MILTON_IS_HANDLE_VALID(m_handle));
   ASSERT(m_platform != NULL);
}

SharedLibrary::~SharedLibrary() 
{ }

void *SharedLibrary::getSymbol(const std::string &symbol) {
   return m_platform->getSymbol(m_handle, symbol);
}

std::string SharedLibrary::getLastError() {
   return m_platform->getLastError();
}

}

