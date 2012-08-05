/**<!-------------------------------------------------------------------->
   @class  MiltonJSONSceneLoader
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008
   
   @brief
      Scene loader for Milton JSON scene format
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_JSON_SCENE_LOADER_H_
#define MILTON_JSON_SCENE_LOADER_H_

#include <loaders/SceneLoader.h>
#include <loaders/SceneGraph.h>
#include <dynamic/DynamicPlugin.h>

#include <materials/Material.h>
#include <shapes/ShapeSet.h>

#include <third-party/tinyjson/tinyjson.hpp>
#include <typeinfo>

namespace milton {

struct JSONParseData;
struct JSONVisitor;
class  MiltonJSONSceneLoader;
class  KernelFilter;

DECLARE_STL_TYPEDEF2(std::map<std::string, SceneNode*>,    InstancedNodeMap);

DECLARE_STL_TYPEDEF2(std::map<std::string, std::string>, KeyValueIDMap);

DECLARE_STL_TYPEDEF2(std::map<std::string, unsigned>, ParseResultsMap);
   
DECLARE_STL_TYPEDEF(json::grammar<char>::array,        JSONArray);
DECLARE_STL_TYPEDEF(json::grammar<char>::object,       JSONObject);
DECLARE_STL_TYPEDEF(std::string,                       JSONString);
typedef             json::grammar<char>::variant       JSONVariant;

typedef bool (MiltonJSONSceneLoader::*ParseFunction) 
   (const JSONVariant &, JSONParseData &) const;

struct MILTON_DLL_EXPORT JSONParseData : public ParseData {
   ShapeSet        *primitives;
   MaterialList     materials;
   
   Material        *activeMaterial;
   Material        *background;
   
   // temporary during parsing (stack of SceneNodes)
   SceneGraph       sceneGraph;
   
   InstancedNodeMap instancedNodes;
   unsigned         instance;
   
   KernelFilter    *filter;
   
   JSONParseData(const ParseData &copy)
      : ParseData(copy), primitives(NULL), activeMaterial(NULL), 
        background(NULL), instance(0), filter(NULL)
   { }
   
   /// clears the SceneGraph and frees its resources
   virtual void reset() {
      ParseData::reset();
      
      instancedNodes.clear();
      safeDelete(primitives);
      materials.clear();
      
      activeMaterial = NULL;
      background     = NULL;
      instance       = 0;
      filter         = NULL;
      
      sceneGraph.reset();
   }
};

struct MILTON_DLL_EXPORT JSONVisitorValue {
   ParseFunction function;
   bool          required;
   
   inline JSONVisitorValue(const ParseFunction &function_ = NULL, 
                           const bool required_ = false)
      : function(function_), required(required_)
   { }
};

/*struct JSONVisitorConstraint {
   const std::string &key;
   bool               required;
   
   inline JSONVisitorConstraint(const std::string &key_, 
                                const bool required_ = false)
      : key(key_), required(required_)
   { }
};*/

struct MILTON_DLL_EXPORT JSONVisitor {
   DECLARE_STL_TYPEDEF2(std::map<std::string, JSONVisitorValue>, FunctionMap);
   
   FunctionMap functionMap;
   
   JSONVisitor(ParseFunction nullFunction, 
               ParseFunction boolFunction, 
               ParseFunction intFunction, 
               ParseFunction real_tFunction, 
               ParseFunction stringFunction, 
               ParseFunction arrayFunction, 
               ParseFunction objectFunction)
   {
/*#define ADD_VARIANT(v, type) \
   if (v##Function)          \
      functionMap[std::string(typeid(type).name())] = JSONVisitorValue(v##Function, true);
      
      ADD_VARIANT(null,   void);
      ADD_VARIANT(bool,   bool);
      ADD_VARIANT(int,    int);
      ADD_VARIANT(real_t, real_t);
      ADD_VARIANT(string, JSONString);
      ADD_VARIANT(array,  JSONArray);
      ADD_VARIANT(object, JSONObject);
      
#undef ADD_VARIANT*/
      if (nullFunction) functionMap["void"] = JSONVisitorValue(nullFunction, true);
      if (boolFunction) functionMap["bool"] = JSONVisitorValue(boolFunction, true);
      if (intFunction)  functionMap["int"]  = JSONVisitorValue(intFunction,  true);
      if (real_tFunction) functionMap["real_t"] = JSONVisitorValue(real_tFunction, true);
      if (stringFunction) functionMap["string"] = JSONVisitorValue(stringFunction, true);
      if (arrayFunction)  functionMap["array"]  = JSONVisitorValue(arrayFunction,  true);
      if (objectFunction) functionMap["object"] = JSONVisitorValue(objectFunction, true);
   }
   
   inline JSONVisitor()
   { }
   
   inline JSONVisitorValue &operator[](const std::type_info &type) {
      return (*this)[type.name()];
   }
   
   inline JSONVisitorValue &operator[](const std::string &key) {
      return functionMap[key];
   }
   
   // perform the functions defined by this JSONVisitor on the given variant, 
   // depending on its type
   bool visit(const MiltonJSONSceneLoader *loader, const JSONVariant &v, 
              JSONParseData &data);
   
   // iterate through each key, value (string : variant) pair in the given 
   // JSONObject, invoking the action specified by this JSONVisitor on each 
   // sub-variant
   bool visit(const MiltonJSONSceneLoader *loader, JSONObject const &obj, 
              JSONParseData &data, ParseResultsMap *results = NULL, 
              bool allowRepeats = false, bool atLeastOne = false, 
              // strict determines whether or not an error occurs if an 
              // unknown key is encountered
              bool strict = true);
};

class MILTON_DLL_EXPORT MiltonJSONSceneLoader : public SceneLoader {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MiltonJSONSceneLoader()
         : SceneLoader()
      { }
      
      virtual ~MiltonJSONSceneLoader()
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
      virtual bool parse(const std::string &fileName, ParseData &outData);
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual bool _parse(const std::string &input,   ParseData &outData) const;
      
      /**
       * @brief
       *    Called after successful parsing of 'scenefile' element
       * 
       * Initializes defaults for the camera, output, and renderer of they 
       * weren't specified and ensures all references are in place between 
       * them
       */
      virtual bool _finalizeData(JSONParseData &data) const;
      
   private:
#define ADD_PARSE_METHOD(name) \
   bool _parse_##name(const JSONVariant &v, JSONParseData &data) const;
      
      ADD_PARSE_METHOD(root);
      ADD_PARSE_METHOD(scenefile);
      
      ADD_PARSE_METHOD(version);
      
      ADD_PARSE_METHOD(renderer);
      ADD_PARSE_METHOD(camera);
      ADD_PARSE_METHOD(output);
      
      ADD_PARSE_METHOD(scene);
      
      ADD_PARSE_METHOD(transform);
      ADD_PARSE_METHOD(shape);
      ADD_PARSE_METHOD(material);
      
      ADD_PARSE_METHOD(background);
      
      ADD_PARSE_METHOD(bsdf);
      ADD_PARSE_METHOD(emitter);
      ADD_PARSE_METHOD(medium);
      
      ADD_PARSE_METHOD(translate);
      ADD_PARSE_METHOD(rotate);
      ADD_PARSE_METHOD(scale);
      ADD_PARSE_METHOD(arbitraryTransform);
      
      ADD_PARSE_METHOD(blob);
      ADD_PARSE_METHOD(cube);
      ADD_PARSE_METHOD(cone);
      ADD_PARSE_METHOD(cylinder);
      ADD_PARSE_METHOD(plane);
      ADD_PARSE_METHOD(point);
      ADD_PARSE_METHOD(mesh);
      ADD_PARSE_METHOD(shapeSet);
      ADD_PARSE_METHOD(sphere);
      ADD_PARSE_METHOD(triangle);
      
      ADD_PARSE_METHOD(spatialAccel);
      
      ADD_PARSE_METHOD(metaObjects);
      ADD_PARSE_METHOD(meta);
      ADD_PARSE_METHOD(ball);
      
      ADD_PARSE_METHOD(dummy);
      
      // helper utilities
      bool _parse_properties(const KeyValueIDMap &constraints,
                             JSONObject const &obj, 
                             PropertyMap &outMap, 
                             JSONParseData &data) const;
      
      bool _parse_emitter_helper(const JSONVariant &v, JSONParseData &data, 
                                 bool background) const;
      
      bool _parse_filter(const JSONVariant &v, JSONParseData &data) const;
      
      void _add_node(JSONVisitor &outVisitor) const;
      void _add_shapes(JSONVisitor &outVisitor) const;
      
      bool _parse_real_t(const JSONVariant &v, real_t &outVal) const;
      bool _parse_real_t_array(const JSONVariant &v, real_t *outArray, 
                               unsigned expectedLength, 
                               JSONParseData &data) const;
      
      inline bool _is_valid_clampedSpectralSampleSet(const real_t s) const {
         return (s >= 0 && s <= 1);
      }
      
      template<typename T, unsigned E>
      T *_parse_dynamic_plugin(JSONObject const &obj, JSONParseData &data) const;
};

}

#endif // MILTON_JSON_SCENE_LOADER_H_

