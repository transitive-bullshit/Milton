/**<!-------------------------------------------------------------------->
   @class  IFeatures
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Utility base class for managing large amounts of (possibly) optional 
   functionality in a clean and consistent manner. The template type 
   'Feature' needs to support bitwise computations.
   <!-------------------------------------------------------------------->**/

#ifndef IFEATURES_H_
#define IFEATURES_H_

#include <common/common.h>

namespace milton {

typedef long Feature;

class MILTON_DLL_EXPORT IFeatures {
   public:
      virtual ~IFeatures() { }
      
      /**
       * @brief
       *    Initializes nonstandard features to speedup performance.
       *
       * If some or all of the requested features have already been 
       * initialized, they can optionally be reinitialized by setting 
       * 'forceInit' to true
       *
       * @returns whether or not requested features were all successfully 
       *    initialized
       */
      virtual bool initializeFeatures(Feature features = Feature(), bool forceInit = false) = 0;
      
      /**
       * @brief
       *    Cleanups any data associated with the given features and 
       * disables them until they are reinitialized by initializeFeatures
       */
      virtual void cleanupFeatures(Feature features = Feature()) = 0;
      
      /**
       * @returns all of the features supported by this class ORed together
       */
      virtual Feature getSupportedFeatures() const = 0;
      
      /**
       * @returns all of the features which have been initialized by a call 
       *    to initializeFeatures
       */
      virtual Feature getInitializedFeatures() const = 0;
      
      /**
       * @returns all of the features which are not currently initialized
       */
      inline Feature getUninitializedFeatures() const {
         return (getSupportedFeatures() & ~getInitializedFeatures());
      }
      
      /**
       * @returns whether or not all of the requrested features have been 
       *    initialized
       */
      inline bool areFeaturesInitialized(Feature requestedFeatures) const {
         Feature initializedFeatures = getInitializedFeatures();
         
         return ((initializedFeatures & requestedFeatures) == requestedFeatures);
      }
      
      /**
       * @returns whether or not all of the requrested features have been 
       *    initialized (pseudonym for areFeaturesInitialized)
       */
      inline bool hasFeature(Feature requestedFeatures) const {
         return areFeaturesInitialized(requestedFeatures);
      }
      
   protected:
      inline IFeatures()
         : m_features(Feature())
      { }
       
   protected:
      Feature m_features;
};

}

#endif // IFEATURES_H_

