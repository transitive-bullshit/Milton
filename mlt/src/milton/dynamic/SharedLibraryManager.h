/*<!-------------------------------------------------------------------->
   @class  SharedLibraryPlatform
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Platform-independent, public interface to loading and unloading an 
   arbitrary number of shared libraries.
   <!-------------------------------------------------------------------->**/

#ifndef SHARED_LIBRARY_MANAGER_H_
#define SHARED_LIBRARY_MANAGER_H_

#include <dynamic/SharedLibraryCommon.h>
#include <map>

namespace milton {

class SharedLibraryPlatform;
class SharedLibrary;

struct MILTON_DLL_EXPORT SharedLibraryReference {
   SharedLibraryHandle handle;
   int                 refCount;

   inline SharedLibraryReference(SharedLibraryHandle h = NULL, int r = 0)
      : handle(h), refCount(r)
   { }
   
   inline SharedLibraryReference &operator=(const SharedLibraryReference &rhs)
   {
      handle   = rhs.handle;
      refCount = rhs.refCount;
      
      return *this;
   }
};

DECLARE_STL_TYPEDEF2(std::map<std::string, SharedLibraryReference>, 
                     SharedLibraryMap);

class MILTON_DLL_EXPORT SharedLibraryManager {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   SharedLibraryManager(SharedLibraryPlatform *platform)
         : m_platform(platform)
      {
         ASSERT(m_platform != NULL);
      }
      
      virtual ~SharedLibraryManager()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Attempts to load/initialize the given shared library, optionally 
       * specifying the given open flags to the platform-dependent loader.
       *
       * SharedLibraries are reference-counted, so if the specified library 
       * is already loaded, the a second copy of the wrapper SharedLibrary 
       * will be returned and the reference count will be incremented 
       * accordingly.
       * 
       * @returns a SharedLibrary wrapper on success or NULL on failure
       */
      virtual SharedLibrary *load(const std::string &name, int openFlags = 0);
      
      /**
       * @brief
       *    Decrements the reference count on the given shared library. The 
       * library will only be unloaded once no more references to it exist.
       *
       * @returns true on successful decrement, false otherwise
       */
      virtual bool unload(const std::string &name);
      
      /**
       * @returns whether or not the given library is currently loaded with 
       * respect to this manager.
       */
      virtual bool isLoaded(const std::string &name) const;
      
      /**
       * @brief
       *    Searches all shared libraries that are currently loaded for the 
       * given symbol
       * 
       * @returns the handle of the shared library containing the given symbol 
       *    or NULL if the symbol could not be found
       */
      virtual void *getSymbol(const std::string &symbol);
      
      /**
       * @returns a human-readable string describing the last error which 
       *    occurred within this SharedLibrary
       */
      virtual std::string getLastError();
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      /// Maps library names to reference counts and platform-dependent handles
      SharedLibraryMap m_map;
      
      /// Platform-dependent interface to low-level DSO routines
      SharedLibraryPlatform *m_platform;
};

}

#endif // SHARED_LIBRARY_MANAGER_H_

