/**<!-------------------------------------------------------------------->
   @class  ResourceManager
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

#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <utils/PropertyMap.h>
#include <common/image/Image.h>
#include <utils/Log.h>

#include <QtCore/QThreadStorage>
#include <QtCore/QMutex>

namespace milton {

DECLARE_STL_TYPEDEF2(std::map<std::string, ImagePtr>, ImagePtrMap);

typedef QThreadStorage<PropertyMap*> ThreadLocalStorage;

class SharedLibraryManager;
class SharedLibraryPlatform;

class MILTON_DLL_EXPORT ResourceManager {
   public:
      ///@name External image cache
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Loads the requested image from the given file, retaining a 
       * global cache of all previously loaded images to default to 
       * upon future calls to 'getImage'
       * 
       * @returns a reference to an Image upon success or an empty 
       *    reference upon failure
       */
      static ImagePtr getImage(const std::string &filename) {
         QMutexLocker lock(&s_mutex);
         
         // attempt to find previously loaded, image in cache
         ImagePtrMapIter iter = s_imagePtrMap.find(filename);
         
         if (iter != s_imagePtrMap.end())
            return iter->second;
         
         ImagePtr imagePtr = ImagePtr(Image::load(filename));
         
         if (!imagePtr) {
            ResourceManager::log.error << "failed to load image '" << 
               filename << "'" << std::endl;
         }
         
         s_imagePtrMap.insert(ImagePtrMap::value_type(filename, imagePtr));
         return imagePtr;
      }
      
      /**
       * @brief
       *    Attempts to synchronously clean up all resources which are not 
       * currently in use, flushing the static cache of any currently 
       * unused images.
       */
      static void cleanup();
      
      
      //@}-----------------------------------------------------------------
      ///@name Thread-local resources map
      //@{-----------------------------------------------------------------
      
      static bool containsThreadLocal(const std::string &key) {
         const PropertyMap *properties = 
            (const PropertyMap *) s_threadLocalStorage.localData();
         
         if (NULL == properties)
            return false;
         
         return properties->contains(key);
      }
      
      template <typename T>
      static void insertThreadLocal(const std::string &key, const T &value) {
         PropertyMap *properties = _getThreadLocalProperties();
         ASSERT(properties);
         
         properties->insert<T>(key, value);
         ASSERT(properties->getValue<T>(key) == value);
      }
      
      template <typename T>
      static T &getValueThreadLocal(const std::string &key) {
         PropertyMap *properties = _getThreadLocalProperties();
         ASSERT(properties);
         
         return properties->getValue<T>(key);
      }
      
      template <typename T>
      static T &getValueThreadLocal(const std::string &key, const T &defaultValue) {
         PropertyMap *properties = _getThreadLocalProperties();
         ASSERT(properties);
         
         return properties->getValue<T>(key, defaultValue);
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Global logging interface
      //@{-----------------------------------------------------------------
      
      static Log log;
      
      
      //@}-----------------------------------------------------------------
      ///@name Global user-definable options
      //@{-----------------------------------------------------------------
      
      static bool contains(const std::string &key) {
         QMutexLocker lock(&s_mutex);
         
         return s_propertyMap.contains(key);
      }
      
      template <typename T>
      static void insert(const std::string &key, const T &value) {
         QMutexLocker lock(&s_mutex);
         
         s_propertyMap.insert<T>(key, value);
      }
      
      template <typename T>
      static T &getValue(const std::string &key) {
         QMutexLocker lock(&s_mutex);
         
         return s_propertyMap.getValue<T>(key);
      }
      
      template <typename T>
      static T &getValue(const std::string &key, const T &defaultValue) {
         QMutexLocker lock(&s_mutex);
         
         return s_propertyMap.getValue<T>(key, defaultValue);
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Global external shared library manager (for plugins)
      //@{-----------------------------------------------------------------
      
      static SharedLibraryManager *manager;
      
      
      //@}-----------------------------------------------------------------
      
   private:
      static PropertyMap *_getThreadLocalProperties() {
         PropertyMap *properties = (PropertyMap *) s_threadLocalStorage.localData();
         
         if (NULL == properties) {
            properties = new PropertyMap();
            s_threadLocalStorage.setLocalData(properties);
         }
         
         return properties;
      }
      
   private:
      static ImagePtrMap            s_imagePtrMap;
      static ThreadLocalStorage     s_threadLocalStorage;
      static PropertyMap            s_propertyMap;
      static QMutex                 s_mutex;
      static SharedLibraryPlatform *s_sharedLibraryPlatform;
};

}

#endif // RESOURCE_MANAGER_H_

