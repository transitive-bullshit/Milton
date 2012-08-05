/*<!-------------------------------------------------------------------->
   @file   SharedLibraryPlatform.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Platform-independent, public interface to loading and unloading an 
   arbitrary number of shared libraries.
   <!-------------------------------------------------------------------->**/

#include "SharedLibraryManager.h"
#include "SharedLibraryPlatform.h"
#include "SharedLibrary.h"

namespace milton {

SharedLibrary *SharedLibraryManager::load(const std::string &name, 
                                          int openFlags)
{
   if (!isLoaded(name)) {
      SharedLibraryHandle handle = m_platform->load(name, openFlags);
      
      // Check for load errors
      if (!MILTON_IS_HANDLE_VALID(handle))
         return NULL;
      
      // Create a new entry in our library map
      m_map[name] = SharedLibraryReference(handle, 0);
   }
   
   // Increment refcount on reference entry
   m_map[name].refCount++;
   
   return new SharedLibrary(name, m_map[name].handle, m_platform);
}

bool SharedLibraryManager::unload(const std::string &name) {
   bool ret = false;
   
   if (isLoaded(name) && --m_map[name].refCount < 0) {
      ret = m_platform->unload(m_map[name].handle);
      m_map.erase(m_map.find(name));
   }
   
   return ret;
}

bool SharedLibraryManager::isLoaded(const std::string &name) const {
   return (m_map.find(name) != m_map.end());
}

void *SharedLibraryManager::getSymbol(const std::string &symbol) {
   SharedLibraryMapConstIter iter;
   void *handleToSymbol = NULL;
   
   for(iter = m_map.begin(); iter != m_map.end(); iter++) {
      handleToSymbol = m_platform->getSymbol(iter->second.handle, symbol);
      
      if (handleToSymbol != NULL)
         break;
   }
   
   return handleToSymbol;
}

std::string SharedLibraryManager::getLastError() {
   return m_platform->getLastError();
}

}

