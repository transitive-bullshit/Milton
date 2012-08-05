/*<!-------------------------------------------------------------------->
   @class  SharedLibraryPlatform
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Platform-independent interface for low-level operations on shared 
   libraries, including platform-dependent implementations for POSIX and 
   Windows systems.
   
   @note Shared libraries are typically denoted with *.so filenames on POSIX-
      compliant platforms and with *.dll or *.DLL on Windows.
   <!-------------------------------------------------------------------->**/

#ifndef SHARED_LIBRARY_PLATFORM_H_
#define SHARED_LIBRARY_PLATFORM_H_

#include <dynamic/SharedLibraryCommon.h>

namespace milton {

class MILTON_DLL_EXPORT SharedLibraryPlatform {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   SharedLibraryPlatform()
      { }
      
      virtual ~SharedLibraryPlatform()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Attempts to load/initialize the given shared library, optionally 
       * specifying the given open flags to the platform-dependent loader
       *
       * @returns a platform-dependent handle to the library on success 
       *          or NULL on failure
       */
      virtual SharedLibraryHandle load(const std::string &name, 
                                       int openFlags = 0) = 0;
      
      /**
       * @brief
       *    Attempts to unload the given shared library
       * 
       * @returns true on success, false otherwise
       */
      virtual bool unload(SharedLibraryHandle handle) = 0;
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a reference to the given symbol in this SharedLibrary or 
       *    NULL on error
       * 
       * @note if an error occurred, you may call getLastError to obtain a 
       *    human-readable string describing the error
       */
      virtual void *getSymbol(SharedLibraryHandle handle, 
                              const std::string &symbol) = 0;
      
      /**
       * @returns a human-readable string describing the last error which 
       *    occurred within this SharedLibrary
       */
      virtual std::string getLastError() = 0;
      
      
      //@}-----------------------------------------------------------------
};


#ifdef MILTON_ARCH_WINDOWS

/**
 * @brief
 *    Windows-compliant DLL interface
 */
class MILTON_DLL_EXPORT SharedLibraryPlatformWindows : public SharedLibraryPlatform {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   SharedLibraryPlatformWindows()
         : m_lastError("")
      { }
      
      virtual ~SharedLibraryPlatformWindows()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /// Wrapper around dlopen
      virtual SharedLibraryHandle load(const std::string &name, 
                                       int openFlags = 0);
      
      /// Wrapper around dlclose
      virtual bool unload(SharedLibraryHandle handle);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      /// Wrapper around dlsym
      virtual void *getSymbol(SharedLibraryHandle handle, 
                              const std::string &symbol);
      
      /// Wrapper around dlerror
      virtual std::string getLastError();
      
      
      //@}-----------------------------------------------------------------

   protected:
      std::string m_lastError;
};

#else

/**
 * @brief
 *    POSIX-compliant dl* interface (dlopen, dlsym, dlclose, dlerror...)
 * 
 * @note
 *    Meant for use on UNIX derivatives
 */
class MILTON_DLL_EXPORT SharedLibraryPlatformPosix : public SharedLibraryPlatform {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   SharedLibraryPlatformPosix()
      { }
      
      virtual ~SharedLibraryPlatformPosix()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /// Wrapper around dlopen
      virtual SharedLibraryHandle *load(const std::string &name, 
                                        int openFlags = 0);
      
      /// Wrapper around dlclose
      virtual bool unload(SharedLibraryHandle *handle);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      /// Wrapper around dlsym
      virtual void *getSymbol(SharedLibraryHandle *handle, 
                              const std::string &symbol);
      
      /// Wrapper around dlerror
      virtual std::string getLastError();
      
      
      //@}-----------------------------------------------------------------
};

#endif

}

#endif // SHARED_LIBRARY_PLATFORM_H_

