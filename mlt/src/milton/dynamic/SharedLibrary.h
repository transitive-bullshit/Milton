/*<!-------------------------------------------------------------------->
   @class  SharedLibrary
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Platform-independent utility wrapper around a dynamically loaded shared 
   library.
   <!-------------------------------------------------------------------->**/

#ifndef SHARED_LIBRARY_H_
#define SHARED_LIBRARY_H_

#include <dynamic/SharedLibraryCommon.h>

namespace milton {

class SharedLibraryPlatform;

class MILTON_DLL_EXPORT SharedLibrary {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Creates a utility wrapper around the handle given which is assumed 
       * to be a valid platform-dependent handle to a previously loaded shared 
       * library
       */
      SharedLibrary(const std::string &name, SharedLibraryHandle handle, 
                    SharedLibraryPlatform *platform);
      
      virtual ~SharedLibrary();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a reference to the given symbol in this SharedLibrary or 
       *    NULL on error
       * 
       * @note if an error occurred, you may call getLastError to obtain a 
       *    human-readable string describing the error
       */
      virtual void *getSymbol(const std::string &symbol);
      
      /**
       * @returns a human-readable string describing the last error which 
       *    occurred within this SharedLibrary
       */
      virtual std::string getLastError();
      
      /**
       * @returns the string used to find/load this SharedLibrary
       */
      inline const std::string &getName() const {
         return m_name;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      const std::string      m_name;
      
      SharedLibraryHandle    m_handle;
      SharedLibraryPlatform *m_platform;
};

}

#endif // SHARED_LIBRARY_H_

