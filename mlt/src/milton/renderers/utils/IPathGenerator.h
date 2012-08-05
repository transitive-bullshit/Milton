/**<!-------------------------------------------------------------------->
   @class  IPathGenerator
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Interface responsible for generating valid light-to-eye paths
   
   @see also PathTracer and BidirectionalPathTracer
   <!-------------------------------------------------------------------->**/

#ifndef IPATH_GENERATOR_H_
#define IPATH_GENERATOR_H_

#include <renderers/utils/Path.h>

namespace milton {

class MILTON_DLL_EXPORT IPathGenerator {
   public:
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Generates a complete, valid path from scratch
       * 
       * @note path generated is assumed to start at an emitter (light) and 
       *    end at a sensor (camera)
       * @returns true on success or false on failure (if an invalid path 
       *    was encountered)
       */
      virtual bool generate(Path &outPath) = 0;
      
      /**
       * @brief
       *    Generates a light subpath which is assumed to start at an emitter
       */
      virtual void generateL(Path &outPath) = 0;
      
      /**
       * @brief
       *    Generates an eye subpath which is assumed to end at a sensor
       */
      virtual void generateE(Path &outPath) = 0;
      
      //@}-----------------------------------------------------------------
};

}

#endif // IPATH_GENERATOR_H_

