/**<!-------------------------------------------------------------------->
   @file   ResourceManager.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Static resource manager synchronized across all Milton threads, 
   containing references to loaded images, global logging utilities, and 
   user-definable options which may be used to affect Milton functionality.
   Many user-definable options that are supported are aimed at dynamic, 
   on-the-fly debugging changes (whether or not to preview kd-Trees built 
   from Meshes, for instance)
   <!-------------------------------------------------------------------->**/

#include "ResourceManager.h"
#include <SharedLibraryManager.h>
#include <SharedLibraryPlatform.h>
#include <QtCore/QtCore>

namespace milton {

// initialize static members of ResourceManager
Log ResourceManager::log;

ImagePtrMap             ResourceManager::s_imagePtrMap;
ThreadLocalStorage      ResourceManager::s_threadLocalStorage;
PropertyMap             ResourceManager::s_propertyMap;
QMutex                  ResourceManager::s_mutex;
SharedLibraryPlatform  *ResourceManager::s_sharedLibraryPlatform = 
#ifdef _WIN32
   new SharedLibraryPlatformWindows();
#else
   new SharedLibraryPlatformPosix();
#endif

SharedLibraryManager   *ResourceManager::manager = 
   new SharedLibraryManager(ResourceManager::s_sharedLibraryPlatform);


void ResourceManager::cleanup() {
   QMutexLocker lock(&s_mutex);
   
   std::vector<std::string> toRemove;
   
   FOREACH(ImagePtrMapIter, s_imagePtrMap, iter) {
      if (iter->second.use_count() <= 1)
         toRemove.push_back(iter->first);
   }
   
   FOREACH(std::vector<std::string>::iterator, toRemove, iter) {
      s_imagePtrMap.erase(*iter);
   }
   
   // note s_threadLocalStorage takes care of itself as QThreadStorage takes 
   // ownership of all thread-specific data
}

}

