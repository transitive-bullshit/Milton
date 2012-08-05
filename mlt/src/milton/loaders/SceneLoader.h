/**<!-------------------------------------------------------------------->
   @class  SceneLoader
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008

   @brief
      Abstract scene loader
   <!-------------------------------------------------------------------->**/

#ifndef SCENE_LOADER_H_
#define SCENE_LOADER_H_

#include <renderers/Renderer.h>
#include <utils/PropertyMap.h>
#include <utils/Log.h>

namespace milton {

class RenderOutput;
class Renderer;
class Camera;
class ShapeSet;

/**
 * @brief
 *    Wrapper for data which may be loaded externally
 */
struct MILTON_DLL_EXPORT ParseData : public Log {
   // main outputs
   Renderer     *renderer;
   Camera       *camera;
   
   RenderOutput *output;
   PropertyMap   outputProperties;
   
   // version of the milton scene format being parsed
   real_t        version;
   
   // source filename or description of source being parsed
   std::string   source;
   
   // error message if parsing goes awry
   std::string   error;
   
   // (default log is standard output when debug logging is enabled)
   inline ParseData(bool debug = false)
      : Log(debug), 
        renderer(NULL), camera(NULL), output(NULL), 
        version(0), source(""), error("")
   { }
   
   virtual ~ParseData()
   { }
   
   inline ParseData &operator=(ParseData &data) {
      *(static_cast<Log*>(this)) = data;
      
      renderer = data.renderer;
      camera   = data.camera;
      output   = data.output;
      
      outputProperties.inherit(data.outputProperties);
      
      error    = data.error;
      
      return *this;
   }
   
   void reset() {
      renderer = NULL;
      output   = NULL;
      camera   = NULL;
      outputProperties.clear();
      
      error    = "";
      init();
   }
};

class MILTON_DLL_EXPORT SceneLoader : public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      SceneLoader()
         : PropertyMap()
      { }
      
      virtual ~SceneLoader()
      { }
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Attempts to parse the given file, storing the results in outData
       * 
       * @returns whether or not parsing was successful
       */
      virtual bool parse(const std::string &fileName, ParseData &outData) = 0;
      
      //@}-----------------------------------------------------------------
};

}

#endif // SCENE_LOADER_H_

