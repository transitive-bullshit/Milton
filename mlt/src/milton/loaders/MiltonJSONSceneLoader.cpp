/**<!-------------------------------------------------------------------->
   @file   MiltonJSONSceneLoader.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008
   
   @brief
      Scene loader for Milton JSON scene format
   <!-------------------------------------------------------------------->**/

#include "MiltonJSONSceneLoader.h"
#include <milton.h>

#include <QtCore/QtCore>
#include <boost/any.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

namespace milton {


#ifdef MILTON_ARCH_UNIX
#define PARSE_ERROR(errorStr)                                \
   do {                                                      \
      std::stringstream s_;                                  \
      s_ << std::string(__func__) << " (" << __LINE__ <<     \
         "): " << std::string((errorStr));                   \
      data.error = s_.str();                                 \
      return false;                                          \
   } while(0)
#else
#define PARSE_ERROR(errorStr)                                \
   do {                                                      \
      std::stringstream s_;                                  \
      s_ << std::string(__FUNCTION__) << " (" << __LINE__ << \
         "): " << std::string((errorStr));                   \
      data.error = s_.str();                                 \
      return false;                                          \
   } while(0)
#endif

#define PARSE_TYPE_ERROR(obj, typeStr, v)                    \
   PARSE_ERROR((((std::string(obj) + " must be of type '") + \
                std::string(typeStr) + "', found '") +       \
                std::string((v)->type().name()) + "'"))

#define PARSE_CHECK_TYPE_ERROR(obj, typeStr, type_, v) \
      do {                                             \
         if ((v)->type() != typeid(type_))           \
            PARSE_TYPE_ERROR(obj, typeStr, v);         \
      } while(0)

#define PARSE_ADD_MAP(map, str, val)                                       \
   do {                                                                    \
      if ((map).contains((str)))                                           \
         PARSE_ERROR("duplicate key '" + std::string((str)) + "' found");  \
      (map)[(str)] = (val);                                                \
   } while(0)

#define PARSE_ADD_MAP_OPTIONAL(map, str, val)                              \
   do {                                                                    \
      if (!(map).contains((str)))                                          \
         (map)[(str)] = (val);                                             \
   } while(0)


bool JSONVisitor::visit(const MiltonJSONSceneLoader *loader, 
                        const JSONVariant &v, 
                        JSONParseData &data)
{
   const std::string &type = (v->empty() ? "void" : v->type().name());
   FunctionMapIter iter = functionMap.find(type);
   ParseFunction visitor;
   
   if ((iter == functionMap.end()) || 
       (NULL == (visitor = iter->second.function)))
      return false;
   
   return ((loader->*(visitor)) (v, data));
}

bool JSONVisitor::visit(const MiltonJSONSceneLoader *loader, 
                        JSONObject const &obj, JSONParseData &data, 
                        ParseResultsMap *results, bool allowRepeats, 
                        bool atLeastOne, bool strict)
{
   unsigned iterations = 0;
   
   FOREACH(JSONObjectConstIter, obj, iter) {
      const std::string &first    = iter->first;
      const JSONVariant &second   = iter->second;
      FunctionMapConstIter iter2  = functionMap.find(first);
      
      if (iter2 == functionMap.end()) {
         if (!strict)
            continue;
         
         PARSE_ERROR("encountered unknown key '" + first + "'");
      }
      
      const JSONVisitorValue &val = iter2->second;
      const ParseFunction &func   = val.function;
      
      if (NULL == func)
         continue; // disregard
      
      data << "<" << first << ">" << endl;
      if (!(loader->*func) (second, data))
         return false;
      
      // optionally record results
      // (number of times each key was successfully parsed)
      if (results && ++((*results)[first]) > 1 && !allowRepeats)
         PARSE_ERROR("found multiple definitions of key '" + first + "'");
      
      ++iterations;
   }
   
   if (results) {
      FOREACH(ParseResultsMapConstIter, *results, iter) {
         const std::string &key = iter->first;
         unsigned noOccurrences = iter->second;
         
         if (noOccurrences <= 0 && functionMap[key].required)
            PARSE_ERROR("missing required key '" + key + "'");
      }
   }
   
   return (!atLeastOne || iterations > 0);
}

bool MiltonJSONSceneLoader::parse(const std::string &fileName, 
                                  ParseData &outData)
{
   outData.reset();
   outData.source = fileName;
   
   outData << "parsing file '" << fileName << "'" << endl;
   std::ifstream inputStream(fileName.c_str());
   
   if (!inputStream) {
      outData.error = "unable to find/read file '" + fileName + "'";
      return false;
   }
   
   // TODO: optimize this sheise
   std::string file;
   char buf[256];
   
   while(!inputStream.getline(buf, 255).eof())
      file += buf;
   
   bool retVal = _parse(file, outData);
   
   if (retVal)
      outData << "done parsing file '" << fileName << "'" << endl;
   else 
      outData << "error parsing file '" << fileName << "'" << endl;
   
   return retVal;
}

bool MiltonJSONSceneLoader::_parse(const std::string &input, 
                                   ParseData &outData) const
{
   const JSONVariant &root = json::parse(input.begin(), input.end());
    
   if (root->type() != typeid(JSONObject)) {
      outData.error = "error parsing file (ill-formed JSON)";
      return false;
   }
   
   JSONParseData data(outData);
   bool retVal = false;
   
   try {
      retVal = _parse_root(root, data);
   } catch(boost::bad_any_cast &e) {
      //data << "caught exception during parsing: " << std::string(e.what()) << endl;
      data.error = std::string("internal error: ") + std::string(e.what());
   }
   
   outData = data;
   return retVal;
}

bool MiltonJSONSceneLoader::_parse_root(const JSONVariant &v, 
                                        JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("root", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   
   ParseResultsMap results;
   JSONVisitor     visitor;
   
   data.reset();
   visitor["scenefile"] = 
      JSONVisitorValue(&MiltonJSONSceneLoader::_parse_scenefile, true);
   
   if (!visitor.visit(this, obj, data, &results))
      return false;
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_scenefile(const JSONVariant &v, 
                                             JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("scenefile", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   
   ParseResultsMap results;
   JSONVisitor     visitor;
   visitor["renderer"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_renderer);
   visitor["camera"]   = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_camera);
   visitor["output"]   = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_output);
   visitor["scene"]    = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_scene, true);
   visitor["version"]  = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_version);
   
   if (!visitor.visit(this, obj, data, &results))
      return false;

   return _finalizeData(data);
}
   
// initialize defaults for the camera, output, and renderer if they weren't 
// specified and ensure all references are in place between them
bool MiltonJSONSceneLoader::_finalizeData(JSONParseData &data) const {
   // traverse, flatten, and verify integrity of scenegraph
   if (!data.sceneGraph.flatten(data))
      return false;
   
   if (ResourceManager::getValue<bool>("forcePreview", false))
      safeDelete(data.renderer);
   
   // initialize default renderer if none was specified
   if (NULL == data.renderer) {
      if (ResourceManager::getValue<bool>("enableGui", true)) {
         ResourceManager::log.info << "defaulting to OpenGLRenderer" << endl;
         data.renderer = new OpenGLRenderer();
      } else {
         ResourceManager::log.info << "defaulting to RayCaster" << endl;
         data.renderer = new RayCaster();
         (*data.renderer)["ambient"] = SpectralRadianceSet::fill(0.1);
      }
   }
   
   // initialize output dimensions
   unsigned width = 480, height = 480;
   bool found = false;
   PropertyMap &outputProperties = data.outputProperties;
   
   if (data.output) {
      const Viewport &v = data.output->getViewport();
      
      width  = v.getWidth();
      height = v.getHeight();
      found  = true;
      
      outputProperties = *data.output;
   }
   
   if (data.outputProperties.contains("size")) {
      const Vector<2, unsigned> &size = 
         data.outputProperties.getValue<Vector<2, unsigned> >("size");
      
      unsigned oldWidth = width, oldHeight = height;
      width  = size[0];
      height = size[1];
      
      if (found && (oldWidth != width || oldHeight != height))
         PARSE_ERROR("conflicting declarations of output size");
      
      found  = true;
   }
   
   if (data.outputProperties.contains("dimensions")) {
      const Vector<2, unsigned> &size = 
         data.outputProperties.getValue<Vector<2, unsigned> >("dimensions");
      
      unsigned oldWidth = width, oldHeight = height;
      width  = size[0];
      height = size[1];
      
      if (found && (oldWidth != width || oldHeight != height))
         PARSE_ERROR("conflicting declarations of output.size and output.dimensions");
      
      found  = true;
   }
   
   if (data.outputProperties.contains("width")) {
      unsigned old = width;
      width = data.outputProperties.getValue<unsigned>("width");
      
      if (found && old != width)
         PARSE_ERROR("conflicting definitions for output width");
   }
   
   if (data.outputProperties.contains("height")) {
      unsigned old = height;
      height = data.outputProperties.getValue<unsigned>("height");
      
      if (found && old != width)
         PARSE_ERROR("conflicting definitions for output height");
   }
   
   data.outputProperties["width"]  = width;
   data.outputProperties["height"] = height;
   
   // ensure camera is valid
   if (NULL == data.camera)
      data.camera = new PinholeCamera();
   
   PointSampleRenderer *r = NULL;
   try {
      r = dynamic_cast<PointSampleRenderer*>(data.renderer);
   } catch(std::bad_cast&) { }
   
   // if we have PointSampleRenderer, initialize its RenderOutput field
   if (r)
      r->setOutput(data.output);
   
   Scene *scene = new Scene(data.materials, data.primitives);
   scene->setBackground(data.background ? 
                        data.background->getEmitter() : 
                        NULL);
   
   data.renderer->setScene (scene);
   data.renderer->setCamera(data.camera);
   
   // initialize camera (may be reinitialized later, but it's not a big deal)
   data.camera->init(scene);
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_scene(const JSONVariant &v, 
                                         JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("scene", "object", JSONObject, v);
   
   return _parse_shapeSet(v, data);
   /*JSONVisitor visitor;
   _add_node(visitor);
   
   // push root scene node
   data.sceneGraph.beginNode(new SceneNodeShape(new ShapeSet(true)));
   
   // recur and visit children
   if (!visitor.visit(this, obj, data))
      return false;
   
   return true;*/
}

bool MiltonJSONSceneLoader::_parse_material(const JSONVariant &v, 
                                            JSONParseData &data) const
{
   // handle null materials
   if (v->empty()) {
      data.sceneGraph.beginNode(new SceneNodeMaterial(new Material()));
      data << "null" << endl;
      data.sceneGraph.endNode();
      return true;
   }
   
   PARSE_CHECK_TYPE_ERROR("material", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   
   JSONVisitor visitor;
   _add_node(visitor);
   
   visitor["bsdf"]    = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_bsdf);
   visitor["emitter"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_emitter);
   visitor["medium"]  = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_medium);
   visitor["filter"]  = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_filter);
   
   Material *material = new Material();
   
   KeyValueIDMap req;
   
   req["bumpMap"]       = "string";
   req["bumpIntensity"] = "real_t";
   req["repeatU"]       = "real_t";
   req["repeatV"]       = "real_t";
   
   // parse required and optional parameters
   if (!_parse_properties(req, obj, *material, data))
      return false;
   
   data.sceneGraph.beginNode(new SceneNodeMaterial(material));
   
   data.filter = NULL;
   if (!visitor.visit(this, obj, data, NULL, false, false, false))
      return false;
   
   if (data.filter) {
      material->setFilter(data.filter);
      data.filter = NULL;
   }
   
   material->init();
   
   data.sceneGraph.endNode();
   return true;
}

bool MiltonJSONSceneLoader::_parse_transform(const JSONVariant &v, 
                                             JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("transform", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   
   JSONVisitor visitor;
   _add_node(visitor);
   
   visitor["arbitraryTransform"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_arbitraryTransform);
   visitor["translate"]          = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_translate);
   visitor["rotate"]             = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_rotate);
   visitor["scale"]              = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_scale);
   
   data.sceneGraph.beginNode(new SceneNodeTransform());
   
   if (!visitor.visit(this, obj, data))
      return false;
   
   data.sceneGraph.endNode();
   return true;
}

bool MiltonJSONSceneLoader::_parse_shape(const JSONVariant &v, 
                                         JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("shape", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter instanceIter = obj.find("instance");
   JSONObjectConstIter typeIter     = obj.find("type");
   std::string type = "";
   
   SceneNode *node = NULL;
   PropertyMap properties;
   KeyValueIDMap req;
   req["name"] = "string";
   
   if (instanceIter != obj.end()) {
      PARSE_CHECK_TYPE_ERROR("shape.instance", "string", 
                             JSONString, instanceIter->second);
      const std::string &instancee = 
         boost::any_cast<JSONString>(*(instanceIter->second));
      data << "instance '" << instancee << "'" << endl;
      type = "instance";
      
      if (!_parse_properties(req, obj, properties, data))
         return false;
      
      node = new SceneNodeInstance(instancee, SCENE_NODE_SHAPE);
   } else {
      if (typeIter == obj.end())
         PARSE_ERROR("unable to infer shape type");
      
      PARSE_CHECK_TYPE_ERROR("shape.type", "string", 
                             JSONString, typeIter->second);
      type = boost::any_cast<JSONString>(*(typeIter->second));
      data << type << endl;
      
      Shape *shape = NULL;
      
      if (type == "plane") {
         req["vertices"] = "vector12";
      } else if (type == "point") {
         shape = new PointShape();
      } else if (type == "cube") {
         shape = new Cube();
      } else if (type == "cone") {
         shape = new Cone();
      } else if (type == "mesh") {
         req["path"]      = "string";
         req["normalize"] = "bool";
      } else if (type == "cylinder") {
         shape = new Cylinder();
      } else if (type == "sphere") {
         shape = new Sphere();
         
         req["position"] = "point3";
         req["radius"]   = "real_t";
      } else if (type == "triangle") {
         req["vertices"] = "vector9";
         req["normals"]  = "vector9"; // per vertex normals
         req["normal"]   = "vector3"; // constant over face of triangle
         req["uvs"]      = "vector6";
      } else if (type == "blob") {
         return _parse_blob(v, data);
      } else if (type == "shapeSet") {
         return _parse_shapeSet(v, data);
      } else if (type == "dynamic") {
         if (!(shape = _parse_dynamic_plugin<Shape, MILTON_PLUGIN_TYPE_SHAPE>(obj, data)))
            PARSE_ERROR("dynamic shape plugin: " + data.error);
      } else {
         PARSE_ERROR("invalid shape type");
      }
      
      // valid variant found; now parse required and optional parameters
      if (!_parse_properties(req, obj, properties, data))
         return false;
      
      if (type == "mesh") {
         if (!properties.contains("path"))
            PARSE_ERROR("missing required key 'path' in 'mesh' (path to mesh data file)");
         
         const std::string &path = properties.getValue<const std::string>("path");
         data << "loading mesh '" << path << "'" << endl;
         Mesh *mesh;
         
         if (NULL == (mesh = MeshLoader::load(path)))
            PARSE_ERROR(std::string("mesh '") + path + std::string("' failed to load"));
         
         mesh->inherit(properties);
         shape = mesh;
         
         if (properties.contains("normalize") && 
             properties.getValue<bool>("normalize"))
         {
            // normalize mesh to reside within the unit cube 
            // [-.5,-.5,-.5] to [.5,.5,.5]
            
            unsigned noVertices = 0;
            Vector3 *vertices = mesh->getVertices(noVertices);
            AABB aabb;
            
            for(unsigned i = noVertices; i--;)
               aabb.add(vertices[i]);
            
            const Vector3 &center = Vector3(aabb.getCenter().data);
            Vector3 diag   = aabb.getDiagonal();
            for(unsigned j = 3; j--;)
               diag[j] += (diag[j] == 0);
            
            for(unsigned i = noVertices; i--;) {
               vertices[i] -= center;
               
               for(unsigned j = 3; j--;)
                  vertices[i][j] /= diag[j];
            }
         }
      } else if (type == "triangle") {
         // defaults
         real_t vertices[9] = {
            0, 0, 0,   // A
            0, 0, 0.5, // B
            0.5, 0, 0, // C
         };
         
         real_t normals[9] = {
            0, 1, 0, 
            0, 1, 0, 
            0, 1, 0, 
         };
         
         real_t uvs[6] = {
            0, 0, 
            1, 0, 
            0, 1, 
         };
         
         if (properties.contains("vertices")) {
            real_t *d = properties.getValue<real_t*>("vertices");
            memcpy(vertices, d, sizeof(real_t) * 9);
            safeDeleteArray(d);
         }
         
         if (properties.contains("normals")) {
            real_t *d = properties.getValue<real_t*>("normals");
            memcpy(normals, d, sizeof(real_t) * 9);
            safeDeleteArray(d);
         }
         
         if (properties.contains("uvs")) {
            real_t *d = properties.getValue<real_t*>("uvs");
            memcpy(uvs, d, sizeof(real_t) * 6);
            safeDeleteArray(d);
         }
         
         if (properties.contains("normal")) {
            const Vector3 &normal = properties.getValue<Vector3>("normal");
            for(unsigned i = 3; i--;)
               memcpy(normals + i * 3, normal.data, sizeof(Vector3));
         }
         
         Triangle *triangle = new Triangle(
            Vertex(vertices), Vertex(vertices + 3), Vertex(vertices + 6), 
            Normal(vertices), Normal(normals  + 3), Normal(normals  + 6), 
            UV(uvs[0], uvs[1]), UV(uvs[2], uvs[3]), UV(uvs[4], uvs[5]));
         
         shape = triangle;
      } else if (type == "sphere") {
         Matrix4x4 trans = Matrix4x4::identity();
         
         if (properties.contains("position")) {
            const Point3 &position = properties.getValue<Point3>("position");
            
            trans *= getTransMat(Vector3(position.data));
         }
         
         if (properties.contains("radius")) {
            const real_t radius = properties.getValue<real_t>("radius") * 2.0;
            
            trans *= getScaleMat(Vector3(radius, radius, radius));
         }
         
         (dynamic_cast<Sphere*>(shape))->setTransToWorld(trans);
      } else if (type == "plane") {
         if (properties.contains("vertices")) {
            real_t *d = properties.getValue<real_t*>("vertices");
            
            Vector3 v1(d), v2(d + 3), v3(d + 6), v4(d + 9);
            safeDeleteArray(d);
            
            data.sceneGraph.beginNode(new SceneNodeShape(new Triangle(v1, v2, v3, 
                                                                      UV(0, 0), 
                                                                      UV(0, 1), 
                                                                      UV(1, 1))));
            data.sceneGraph.endNode();
            
            shape = new Triangle(v1, v3, v4, UV(0, 0), UV(1, 1), UV(1, 0));
         } else {
            shape = new Plane();
         }
      }
      
      if (NULL == shape)
         PARSE_ERROR(type + std::string(" failed to load"));
      
      ASSERT(shape);
      node = new SceneNodeShape(shape);
   }
   
   // record 'named' nodes which may be instanced
   if (properties.contains("name")) {
      const std::string &name = properties.getValue<std::string>("name");
      
      if (data.instancedNodes.find(name) != data.instancedNodes.end()) {
         const std::string &s = 
            std::string("error: duplicate definitions for named node '") + 
            name + "'";
         PARSE_ERROR(s);
      }
      
      data.instancedNodes[name] = node;
   }
   
   JSONVisitor visitor;
   visitor["material"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_material);
   
   data.sceneGraph.beginNode(node);
   
   if (type == "mesh") {
      JSONObjectConstIter spatialAccelIter = obj.find("spatialAccel");
      
      if (spatialAccelIter != obj.end()) {
         if (!_parse_spatialAccel(spatialAccelIter->second, data))
            return false;
      }
   }
   
   if (!visitor.visit(this, obj, data, NULL, false, false, false))
      return false;
   
   data.sceneGraph.endNode();
   return true;
}

bool MiltonJSONSceneLoader::_parse_version(const JSONVariant &v, 
                                           JSONParseData &data) const
{
   return _parse_real_t(v, data.version);
}

bool MiltonJSONSceneLoader::_parse_renderer(const JSONVariant &v, 
                                            JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("renderer", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter typeIter = obj.find("type");
   
   if (typeIter == obj.end())
      PARSE_ERROR("unable to infer renderer type");
   
   if (data.renderer)
      PARSE_ERROR("multiple renderer definitions");
   
   PARSE_CHECK_TYPE_ERROR("renderer.type", "string", JSONString, 
                          typeIter->second);
   const std::string &type = boost::any_cast<JSONString>(*(typeIter->second));
   data << type << endl;
   
   data.source += std::string(" (renderer: ") + type + std::string(")");
   
   KeyValueIDMap req;
   
   req["noDirectSamples"] = "uint";
   req["noRenderThreads"] = "uint";
   req["noSuperSamples"]  = "uint";
   req["directSampleGenerator"] = "string";
   req["generator"]       = "string";
   
   if (type == "preview" || type == "OpenGL") {
      data.renderer = new OpenGLRenderer();
   } else if (type == "rayCaster") {
      data.renderer = new RayCaster();
      
      req["ambient"]  = "spectrum";
   } else if (type == "rayTracer") {
      data.renderer = new WhittedRayTracer();
      
      req["maxDepth"] = "uint";
      req["ambient"]  = "spectrum";
   } else if (type == "pathTracer") {
      data.renderer = new PathTracer();
   } else if (type == "bidirectionalPathTracer" || type == "bidirPathTracer") {
      data.renderer = new BidirectionalPathTracer();
   } /*else if (type == "photonMapper") {
      data.renderer = new PhotonMapper();
      
      req["diffuseNoGatherPhotons"] = "uint";
      req["causticNoGatherPhotons"] = "uint";
      req["diffuseGatherRadius"]    = "real_t";
      req["causticGatherRadius"]    = "real_t";
      req["diffuseNoPhotons"]       = "uint";
      req["causticNoPhotons"]       = "uint";
   }*/ else if (type == "mlt" || type == "MLT") {
      req["maxDepth"] = "uint";
      req["maxConsequtiveRejections"] = "uint";
      req["mltBidirPathMutationProb"] = "real_t";
      req["mltLensSubpathMutationProb"] = "real_t";
      req["mltPerturbationPathMutationProb"] = "real_t";
      
      data.renderer = new MLTRenderer();
   } else if (type == "dynamic") {
      if (!(data.renderer = _parse_dynamic_plugin<Renderer, MILTON_PLUGIN_TYPE_RENDERER>(obj, data)))
         PARSE_ERROR("dynamic renderer plugin: " + data.error);
   } else {
      PARSE_ERROR("invalid renderer type");
   }
   
   ASSERT(data.renderer);
   
   // valid variant found; now parse required and optional parameters
   if (!_parse_properties(req, obj, *data.renderer, data))
      return false;
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_output(const JSONVariant &v, 
                                          JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("output", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter typeIter = obj.find("type");
   
   if (data.output)
      PARSE_ERROR("multiple output definitions");
   
   PropertyMap &properties = data.outputProperties;
   KeyValueIDMap req;
   
   req["width"]  = "uint";
   req["height"] = "uint";
   req["size"]   = "uivec2"; // TODO
   req["dimensions"] = req["size"];
   
   if (typeIter != obj.end() || obj.find("filter") != obj.end()) {
      std::string type = "reconstruction";
      
      if (typeIter != obj.end()) {
         PARSE_CHECK_TYPE_ERROR("output.type", "string", JSONString, 
                                typeIter->second);
         type = boost::any_cast<JSONString>(*(typeIter->second));
      }
      
      data << type << endl;
      req["tonemap"] = "string";
      
      if (type == "naive") {
         data.output = new RenderOutput();
      } else {
         ReconstructionRenderOutput *output = NULL;
         
         if (type == "default") {
            output = new DefaultRenderOutput();
         } else if (type == "reconstruction") {
            output = new ReconstructionRenderOutput();
         } else if (type == "dynamic") {
            if (!(data.output = _parse_dynamic_plugin<RenderOutput, MILTON_PLUGIN_TYPE_OUTPUT>(obj, data)))
               PARSE_ERROR("dynamic output plugin: " + data.error);
            
            try {
               output = dynamic_cast<ReconstructionRenderOutput*>(data.output);
            } catch(std::bad_cast&) { }
         } else {
            PARSE_ERROR("invalid output type");
         }
         
         if (output) {
            JSONObjectConstIter filterIter = obj.find("filter");
            
            data.output = output;
            data.filter = NULL;
            
            if (filterIter != obj.end()) {
               if (!_parse_filter(filterIter->second, data) || NULL == data.filter) {
                  safeDelete(data.filter);
                  
                  return false;
               }
               
               output->setFilter(data.filter);
               data.filter = NULL;
            }
         }
      }
      
      ASSERT(data.output);
      properties = *data.output;
   }
   
   // valid variant found; now parse required and optional parameters
   if (!_parse_properties(req, obj, properties, data))
      return false;
   
   if (data.output) {
      if (data.renderer)
         data.output->setParent(data.renderer);
      
      data.output->init();
   }
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_camera(const JSONVariant &v, 
                                          JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("camera", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter typeIter = obj.find("type");
   
   if (typeIter == obj.end())
      PARSE_ERROR("unable to infer camera type");
   
   PARSE_CHECK_TYPE_ERROR("camera.type", "string", 
                          JSONString, typeIter->second);
   const std::string &type = boost::any_cast<JSONString>(*(typeIter->second));
   data << type << endl;
   
   KeyValueIDMap req;
   
   if (data.camera)
      PARSE_ERROR("multiple camera definitions");
   
   if (type == "pinhole" || type == "thinlens") {
      req["eye"]   = "point3";
      req["look"]  = "vector3";
      req["focus"] = "point3";
      req["up"]    = "vector3";
      req["heightAngle"] = "real_t";
      req["aspectRatio"] = "real_t";
      req["near"]  = "real_t";
      req["far"]   = "real_t";
      
      if (type == "thinlens") {
         req["aperture"]      = "real_t";
         req["fstop"]         = "real_t";
         req["focalDistance"] = "real_t";
         req["focalPoint"]    = "vector2";
         
         data.camera = new ThinLensCamera();
      } else {
         data.camera = new PinholeCamera();
      }
   } else if (type == "dynamic") {
      if (!(data.camera = _parse_dynamic_plugin<Camera, MILTON_PLUGIN_TYPE_CAMERA>(obj, data)))
         PARSE_ERROR("dynamic camera plugin: " + data.error);
   } else {
      PARSE_ERROR("invalid camera type");
   }
   
   ASSERT(data.camera);
   
   // valid variant found; now parse required and optional parameters
   if (!_parse_properties(req, obj, *data.camera, data))
      return false;
   
   //data.camera->init();
   // gets initialized later in _finalizeData and/or in Renderer::init
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_bsdf(const JSONVariant &v, 
                                        JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("bsdf", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter typeIter = obj.find("type");
   
   if (typeIter == obj.end())
      PARSE_ERROR("unable to infer bsdf type");
   
   PARSE_CHECK_TYPE_ERROR("bsdf.type", "string", JSONString, 
                          typeIter->second);
   const std::string &type = boost::any_cast<JSONString>(*(typeIter->second));
   data << type << endl;
   
   SceneNodeMaterial *parent = 
      dynamic_cast<SceneNodeMaterial*>(data.sceneGraph.top());
   ASSERT(parent);
   Material *material = parent->material;
   ASSERT(material);
   
   if (material->contains("bsdf"))
      PARSE_ERROR("multiple bsdf definitions");
   
   KeyValueIDMap req;
   
   if (type == "diffuse") {
      material->insert("bsdf", type);
      
      req["kd"] = "clampedSpectrum";
   } else if (type == "specular") {
      material->insert("bsdf", std::string("dielectric"));
      
      req["ks"] = "clampedSpectrum";
   } else if (type == "transmissive") {
      material->insert("bsdf", std::string("dielectric"));
      
      req["ks"] = "clampedSpectrum";
   } else if (type == "dielectric") {
      material->insert("bsdf", type);
      
      req["ks"]           = "clampedSpectrum";
      req["transparency"] = "clampedSpectrum";
   } else if (type == "absorbent") {
      material->insert("bsdf", type);
   } else if (type == "modifiedPhong" || type == "phong") {
      material->insert("bsdf", type);
      
      req["kd"] = "clampedSpectrum";
      req["ks"] = "clampedSpectrum";
      req["n"]  = "spectrum";
   } else if (type == "dynamic") {
      
      NYI(); // TODO
      
   } else {
      PARSE_ERROR("invalid bsdf type");
   }
   
   // valid variant found; now parse required and optional parameters
   if (!_parse_properties(req, obj, *material, data))
      return false;
   
   if (type == "specular")
      material->insert("transparency", 0.0);
   else if (type == "transmissive")
      material->insert("transparency", 1.0);
   
   return true;
}
bool MiltonJSONSceneLoader::_parse_emitter(const JSONVariant &v, 
                                           JSONParseData &data) const
{
   return _parse_emitter_helper(v, data, false);
}

bool MiltonJSONSceneLoader::_parse_emitter_helper(const JSONVariant &v, 
                                                  JSONParseData &data, 
                                                  bool background) const
{
   PARSE_CHECK_TYPE_ERROR("emitter", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter typeIter = obj.find("type");
   
   if (typeIter == obj.end())
      PARSE_ERROR("unable to infer emitter type");
   
   PARSE_CHECK_TYPE_ERROR("emitter.type", "string", JSONString, 
                          typeIter->second);
   const std::string &type = boost::any_cast<JSONString>(*(typeIter->second));
   data << type << endl;
   
   Material *material = NULL;
   
   if (background) {
      material = data.background;
   } else {
      SceneNodeMaterial *parent = 
         dynamic_cast<SceneNodeMaterial*>(data.sceneGraph.top());
      ASSERT(parent);
      material = parent->material;
   }
   
   ASSERT(material);
   
   if (material->contains("emitter"))
      PARSE_ERROR("multiple emitter definitions");
   
   KeyValueIDMap req;
   
   if (type == "null") {
      material->insert("emitter", type);
   } else if (type == "dynamic") {
      
      NYI(); // TODO
      
   } else {
      req["power"]           = "spectrum";
      req["glDiffuseColor"]  = "color3";
      req["glSpecularColor"] = "color3";
      req["glAttenuation"]   = "vector3";
      
      if (type == "omni") {
         material->insert("emitter", type);
      } else if (type == "oriented") {
         material->insert("emitter", type);
      } else if (type == "environment")  {
         if (background) {
            material->insert("emitter", type);
            
            req["coords"] = "string";
            req["path"]   = "string";
         } else {
            PARSE_ERROR("environment map emitters may only be placed in the "
                        "scene's \"background.emitter\" element");
         }
      } else {
         PARSE_ERROR("invalid emitter type");
      }
   }
   
   // valid variant found; now parse required and optional parameters
   if (!_parse_properties(req, obj, *material, data))
      return false;
   
   if (type == "environment" && !material->contains("path")) {
      PARSE_ERROR("environment map must contain a 'path' child element "
                  "specifying an absolute or relative path to an environment "
                  "texture");
   }
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_medium(const JSONVariant &v, 
                                          JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("medium", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter typeIter = obj.find("type");
   std::string type = "homogenous"; // default
   
   if (typeIter != obj.end()) {
      PARSE_CHECK_TYPE_ERROR("medium.type", "string", JSONString, 
                             typeIter->second);
      type = boost::any_cast<JSONString>(*(typeIter->second));
   }
   data << type << endl;
   
   SceneNodeMaterial *parent = 
      dynamic_cast<SceneNodeMaterial*>(data.sceneGraph.top());
   ASSERT(parent);
   Material *material = parent->material;
   ASSERT(material);
   
   if (material->contains("medium"))
      PARSE_ERROR("multiple medium definitions");
   
   KeyValueIDMap req;
   req["ior"] = "spectrum";
   
   if (type == "homogenous") {
      material->insert("medium", type);
   } else if (type == "dynamic") {
      
      NYI(); // TODO
      
   } else {
      PARSE_ERROR("invalid medium type");
   }
   
   // valid variant found; now parse required and optional parameters
   if (!_parse_properties(req, obj, *material, data))
      return false;
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_arbitraryTransform(const JSONVariant &v, 
                                                      JSONParseData &data) const
{
   SceneNodeTransform *parent = 
      dynamic_cast<SceneNodeTransform*>(data.sceneGraph.top());
   
   ASSERT(parent);
   real_t matrix[16];
   
   if (!_parse_real_t_array(v, matrix, 16, data))
      PARSE_ERROR("arbitraryTransform expects 16 doubles in row-major order");
   
   parent->transform *= Matrix4x4(matrix);
   return true;
}

bool MiltonJSONSceneLoader::_parse_translate(const JSONVariant &v, 
                                             JSONParseData &data) const
{
   SceneNodeTransform *parent = 
      dynamic_cast<SceneNodeTransform*>(data.sceneGraph.top());
   
   ASSERT(parent);
   real_t d[3];
   
   if (!_parse_real_t_array(v, d, 3, data))
      PARSE_ERROR("translate transformation expects 3 doubles");
   
   parent->translation *= getTransMat(Vector3(d));
   return true;
}

bool MiltonJSONSceneLoader::_parse_rotate(const JSONVariant &v, 
                                          JSONParseData &data) const
{
   SceneNodeTransform *parent = 
      dynamic_cast<SceneNodeTransform*>(data.sceneGraph.top());
   
   ASSERT(parent);
   real_t d[7];
   
   if (!_parse_real_t_array(v, d, 7, data)) {
      // TODO: attempt to try alternative subobject w/ 
      // 'point', 'axis' and 'angle'
      
      PARSE_ERROR("rotate expects 7 doubles: point specifying origin of rotation; vector specifying axis of rotation, and angle in degrees to rotate about the axis; \n[ px, py, pz, vx, vy, vz, degrees ]");
   }
   
   parent->rotation *= getRotMat(Point3(d[0], d[1], d[2], 1), 
                                 Vector3(d + 3).getNormalized(), DEGREES_TO_RADIANS(d[6]));
   return true;
}

bool MiltonJSONSceneLoader::_parse_scale(const JSONVariant &v, 
                                         JSONParseData &data) const
{
   SceneNodeTransform *parent = 
      dynamic_cast<SceneNodeTransform*>(data.sceneGraph.top());
   
   ASSERT(parent);
   real_t d[3];
   
   if (!_parse_real_t_array(v, d, 3, data))
      PARSE_ERROR("scale expects 3 doubles");
   
   parent->scale *= getScaleMat(Vector3(d));
   return true;
}

bool MiltonJSONSceneLoader::_parse_properties(const KeyValueIDMap &constraints,
                                              JSONObject const &obj, 
                                              PropertyMap &outMap, 
                                              JSONParseData &data) const
{
   std::vector<bool> satisfied(constraints.size(), false);
   
   // parse constrained parameters
   FOREACH(KeyValueIDMapConstIter, constraints, iter) {
      const std::string &key = iter->first;
      // exp is a text description of what is expected in the value of obj
      const std::string &exp = iter->second;
      
      JSONObjectConstIter objIter = obj.find(key);
      
      if (objIter == obj.end()) { // ensure key exists
         //data << "missing optional key '" << std::string(key) << "'" << endl;
         continue;
         //PARSE_ERROR("missing required key '" + std::string(key) + "'");
      }
      
      const JSONVariant &v = objIter->second;
      
      if(v->empty()) {
         if (exp != "null")
            PARSE_TYPE_ERROR(key, exp, v);
         
         PARSE_ADD_MAP(outMap, key, boost::any());
      } else if(v->type() == typeid(bool)) {
         if (exp != "bool")
            PARSE_TYPE_ERROR(key, exp, v);
         
         PARSE_ADD_MAP(outMap, key, boost::any_cast<bool>(*v));
      } else if(v->type() == typeid(int)) {
         int val = boost::any_cast<int>(*v);
         
         if (exp == "int") {
            PARSE_ADD_MAP(outMap, key, val);
         } else if (exp == "real_t") {
            PARSE_ADD_MAP(outMap, key, static_cast<real_t>(val));
         } else if (exp == "clampedSpectrum") {
            const real_t val2 = static_cast<real_t>(val);
            
            if (!_is_valid_clampedSpectralSampleSet(val2))
               PARSE_ERROR("clampedSpectrum must have value within [0, 1]");
            
            PARSE_ADD_MAP(outMap, key, SpectralSampleSet::fill(val2));
         } else if (exp == "spectrum") {
            const real_t val2 = static_cast<real_t>(val);
            
            PARSE_ADD_MAP(outMap, key, SpectralRadianceSet::fill(val2));
         } else if (exp == "uint") {
            PARSE_ADD_MAP(outMap, key, static_cast<unsigned>(val));
         } else {
            PARSE_TYPE_ERROR(key, exp, v);
         }
      } else if(v->type() == typeid(double)) {
         const real_t val = static_cast<real_t>(boost::any_cast<double>(*v));
         
         if (exp == "real_t") {
            PARSE_ADD_MAP(outMap, key, val);
         } else if (exp == "int" || exp == "uint") {
            const int val2 = (int) floor(val);
            if (val2 != val)
               PARSE_ERROR("expected integer; found real_t");
            
            if (exp == "int")
               PARSE_ADD_MAP(outMap, key, val2);
            else 
               PARSE_ADD_MAP(outMap, key, static_cast<unsigned>(val));
         } else if (exp == "clampedSpectrum") {
            if (!_is_valid_clampedSpectralSampleSet(val))
               PARSE_ERROR("clampedSpectrum must have value within [0, 1]");
            
            PARSE_ADD_MAP(outMap, key, SpectralSampleSet::fill(val));
         } else if (exp == "spectrum") {
            PARSE_ADD_MAP(outMap, key, SpectralRadianceSet::fill(val));
         } else {
            PARSE_TYPE_ERROR(key, exp, v);
         }
      } else if(v->type() == typeid(std::string)) {
         if (exp != "string" && exp != "clampedSpectrum")
            PARSE_TYPE_ERROR(key, exp, v);
         
         PARSE_ADD_MAP(outMap, key, boost::any_cast<JSONString>(*v));
      } else if(v->type() == typeid(JSONArray)) {
         JSONArray const &a = boost::any_cast<JSONArray>(*v);
         unsigned length = a.size();
         
         // TODO: allow spectra to have arbitrary length!!
         if (exp == "point3" || exp == "vector3" || exp == "color3" || 
             exp == "spectrum" || exp == "clampedSpectrum")
         {
            if (length != 3)
               PARSE_ERROR("'" + key + "' expects an array of length 3");
            
            real_t d[3];
            for(int i = 0; i < 3; ++i) {
               if (!_parse_real_t(a[i], d[i])) {
                  PARSE_TYPE_ERROR(key, "real_t", a[i]);
               }
            }
            
            if (exp == "point3") {
               PARSE_ADD_MAP(outMap, key, Point3 (d[0], d[1], d[2]));
            } else if (exp == "vector3") {
               PARSE_ADD_MAP(outMap, key, Vector3(d[0], d[1], d[2]));
            } else if (exp == "spectrum") {
               PARSE_ADD_MAP(outMap, key, SpectralRadianceSet(d[0], d[1], d[2]));
            } else if (exp == "clampedSpectrum") {
               for(unsigned i = 0; i < 3; ++i) {
                  if (!_is_valid_clampedSpectralSampleSet(d[i]))
                     PARSE_ERROR("clampedSpectrum expects values within [0, 1]");
               }
               
               PARSE_ADD_MAP(outMap, key, SpectralSampleSet(d[0], d[1], d[2]));
            } else if (exp == "color3") {
               // ensure color components are within [0, 1]
               for(int i = 0; i < 3; ++i) {
                  if (d[i] < 0 || d[i] > 1) {
                     PARSE_ERROR("'" + key + 
                        "' expects a color3 with components in [0, 1]");
                  }
               }
               
               PARSE_ADD_MAP(outMap, key, SpectralRadianceSet(RgbaHDR(d[0], d[1], d[2])));
            } else {
               ASSERT(0 && "PANIC");
            }
         } else if (exp.find("vector") == 0) {
            ASSERT(exp.length() > strlen("vector"));
            
            unsigned n = atoi(exp.c_str() + 6);
            
            if (n != length) {
               ostringstream s;
               s << "'" << key << "' expects an array of length " << n;
               PARSE_ERROR(s.str());
            }
            
            real_t *d = new real_t[n];
            
            for(unsigned i = 0; i < n; ++i) {
               if (!_parse_real_t(a[i], d[i]))
                  PARSE_TYPE_ERROR(key, "real_t", a[i]);
            }
            
            // caller will safeDeleteArray 'd'
            PARSE_ADD_MAP(outMap, key, d);
         } else {
            PARSE_TYPE_ERROR(key, exp, v);
         }
      } else if(v->type() == typeid(JSONObject)) {
         //JSONObject const &o = boost::any_cast<JSONObject>(*v);
         
         ASSERT(exp != "object"); // TODO: currently unhandled
         PARSE_TYPE_ERROR(key, exp, v);
         
      } else {
         // shouldn't ever reach here
         PARSE_ERROR("PANIC: Found unexpected JSON type '" + 
                     std::string(typeid(*v).name()));
      }
   }
   
   // parse optional parameters
   FOREACH(JSONObjectConstIter, obj, iter) {
      const std::string &key = iter->first;
      const JSONVariant &v   = iter->second;
      
      KeyValueIDMapConstIter constraintIter = constraints.find(key);
      
      if (constraintIter != constraints.end()) { // ensure key exists
         // already saw this key in the first FOREACH (it had a type constraint)
         continue;
      }
      
      if(v->empty()) {
         PARSE_ADD_MAP_OPTIONAL(outMap, key, boost::any());
      } else if(v->type() == typeid(bool)) {
         PARSE_ADD_MAP_OPTIONAL(outMap, key, boost::any_cast<bool>(*v));
      } else if(v->type() == typeid(int)) {
         PARSE_ADD_MAP_OPTIONAL(outMap, key, boost::any_cast<int>(*v));
      } else if(v->type() == typeid(double)) {
         PARSE_ADD_MAP_OPTIONAL(outMap, key, 
                                static_cast<real_t>(boost::any_cast<real_t>(*v)));
      } else if(v->type() == typeid(std::string)) {
         PARSE_ADD_MAP_OPTIONAL(outMap, key, boost::any_cast<JSONString>(*v));
      } else if(v->type() == typeid(JSONArray)) {
         JSONArray const &a = boost::any_cast<JSONArray>(*v);
         unsigned length = a.size();
         
         if (length != 3) {
            cout << "ignoring attribute '" << key << "' of irregular length" << endl;
            continue;
         }
         
         real_t d[3];
         for(int i = 0; i < 3; ++i) {
            if (!_parse_real_t(a[i], d[i])) {
               continue;
            }
         }
         
         PARSE_ADD_MAP_OPTIONAL(outMap, key, Vector3(d[0], d[1], d[2]));
      } else if(v->type() == typeid(JSONObject)) {
         //JSONObject const &o = boost::any_cast<JSONObject>(*v);
         
         //PARSE_TYPE_ERROR(key, std::string("optional parameter"), v);
         continue;
      } else {
         // shouldn't ever reach here
         //PARSE_ERROR("PANIC: Found unexpected JSON type '" + 
         //            std::string(objVal));
         continue;
      }
   }
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_filter(const JSONVariant &v, 
                                          JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("filter", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter typeIter = obj.find("type");
   
   if (typeIter == obj.end())
      PARSE_ERROR("unable to infer filter type");
   
   PARSE_CHECK_TYPE_ERROR("filter.type", "string", JSONString, 
                          typeIter->second);
   const std::string &type = boost::any_cast<JSONString>(*(typeIter->second));
   data << type << endl;
   
   KeyValueIDMap req;
   
   req["support"] = "uint";
   
   if (type == "box") {
      data.filter = new BoxFilter();
   } else if (type == "triangle") {
      data.filter = new TriangleFilter();
   } else if (type == "gaussian") {
      data.filter = new GaussianFilter();
      
      req["sigma"] = "real_t";
   } else if (type == "mitchell") {
      data.filter = new MitchellFilter();
      
      req["B"] = "real_t";
      req["C"] = "real_t";
   } else if (type == "lanczosSinc") {
      data.filter = new LanczosSincFilter();
      
      req["tau"] = "real_t";
   } else if (type == "dynamic") {
      if (!(data.filter = _parse_dynamic_plugin<KernelFilter, MILTON_PLUGIN_TYPE_FILTER>(obj, data)))
         PARSE_ERROR("dynamic filter plugin: " + data.error);
   } else {
      PARSE_ERROR("invalid filter type");
   }
   
   ASSERT(data.filter);
   
   // valid variant found; now parse required and optional parameters
   if (!_parse_properties(req, obj, *data.filter, data))
      return false;
   
   data.filter->init();
   return true;
}

void MiltonJSONSceneLoader::_add_node(JSONVisitor &outVisitor) const {
   outVisitor["material"]  = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_material);
   outVisitor["transform"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_transform);
   
   _add_shapes(outVisitor);
}

void MiltonJSONSceneLoader::_add_shapes(JSONVisitor &outVisitor) const {
   // all supported shapes
   // TODO: if we call _parse_shape, lose info about what type of shape it 
   // was!! how to fix cleanly..?
   outVisitor["blob"]     = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_blob);
   outVisitor["cube"]     = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_cube);
   outVisitor["cone"]     = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_cone);
   outVisitor["cylinder"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_cylinder);
   outVisitor["mesh"]     = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_mesh);
   outVisitor["plane"]    = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_plane);
   outVisitor["point"]    = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_point);
   outVisitor["shape"]    = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_shape);
   outVisitor["sphere"]   = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_sphere);
   outVisitor["shapeSet"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_shapeSet);
   outVisitor["triangle"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_triangle);
   
   // TODO
   //outVisitor["dynamic"]  = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_shape);
}

#define DECLARE_VARIANT(prim, shape)                                    \
   bool MiltonJSONSceneLoader::_parse_##prim(const JSONVariant &v,      \
                                             JSONParseData &data) const \
{                                                                       \
   PARSE_CHECK_TYPE_ERROR(#prim, "object", JSONObject, v);              \
   JSONObject obj(boost::any_cast<JSONObject>(*v));                     \
   obj.insert(std::make_pair(JSONString("type"),                        \
                             JSONVariant(                               \
                                new boost::any(std::string(#prim)))));  \
                                                                        \
   return _parse_##shape(JSONVariant(new boost::any(obj)), data);       \
}

#define DECLARE_PRIMITIVE(prim) DECLARE_VARIANT(prim, shape)

DECLARE_PRIMITIVE(cube);
DECLARE_PRIMITIVE(cone);
DECLARE_PRIMITIVE(cylinder);
DECLARE_PRIMITIVE(mesh);
DECLARE_PRIMITIVE(plane);
DECLARE_PRIMITIVE(point);
DECLARE_PRIMITIVE(sphere);
DECLARE_PRIMITIVE(triangle);

bool MiltonJSONSceneLoader::_parse_shapeSet(const JSONVariant &v, 
                                            JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("shapeSet", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   
   JSONVisitor visitor;
   _add_node(visitor);
   
   bool root = (NULL == data.primitives);
   
   if (root)
      visitor["background"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_background);
   
   visitor["spatialAccel"]  = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_spatialAccel);
   
   ShapeSet *shapeSet = new ShapeSet(true);
   if (root)
      data.primitives = shapeSet;
   
   data.sceneGraph.beginNode(new SceneNodeShape(shapeSet));
   
   if (!_parse_properties(KeyValueIDMap(), obj, *shapeSet, data))
      return false;
   
   if (!visitor.visit(this, obj, data, NULL, false, false, false))
      return false;
   
   if (!root)
      data.sceneGraph.endNode();
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_blob(const JSONVariant &v, 
                                        JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("blob", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   
   JSONVisitor visitor;
   _add_node(visitor);
   
   Blob *blob = new Blob();
   KeyValueIDMap req;
   
   req["threshold"] = "real_t";
   
   visitor["metaObjects"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_metaObjects);
   visitor["threshold"] = JSONVisitorValue(&MiltonJSONSceneLoader::_parse_dummy);
   
   data.sceneGraph.beginNode(new SceneNodeShape(blob));
   
   if (!_parse_properties(req, obj, *blob, data))
      return false;
   
   if (!visitor.visit(this, obj, data, NULL, false, false, false))
      return false;
   
   data.sceneGraph.endNode();
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_metaObjects(const JSONVariant &v, 
                                               JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("metaObjects", "array", JSONArray, v);
   JSONArray const &array = boost::any_cast<JSONArray>(*v);
   
   FOREACH(JSONArrayConstIter, array, iter) {
      JSONVariant const &curV = *iter;
      
      PARSE_CHECK_TYPE_ERROR("meta", "object", JSONObject, curV);
      JSONObject const &obj = boost::any_cast<JSONObject>(*curV);
      
      if (obj.find("type") != obj.end()) {
         if (!_parse_meta(curV, data))
            return false;
      } else if (obj.find("ball") != obj.end()) {
         if (!_parse_ball(curV, data))
            return false;
      }
   }
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_meta(const JSONVariant &v, 
                                        JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("meta", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter typeIter = obj.find("type");
   
   if (typeIter == obj.end())
      PARSE_ERROR("unable to infer meta object type");
   
   PARSE_CHECK_TYPE_ERROR("meta.type", "string", 
                          JSONString, typeIter->second);
   const std::string &type = boost::any_cast<JSONString>(*(typeIter->second));
   data << type << endl;
   
   MetaObject *meta = NULL;
   KeyValueIDMap req;
   
   req["negative"] = "bool";
   req["strength"] = "real_t";
   
   if (type == "ball") {
      meta = new MetaBall();
      
      req["position"] = "vector3";
      req["radius"]   = "real_t";
   } else if (type == "dynamic") {
      if (!(meta = _parse_dynamic_plugin<MetaObject, MILTON_PLUGIN_TYPE_METAOBJECT>(obj, data)))
         PARSE_ERROR("dynamic meta-object plugin: " + data.error);
   } else {
      PARSE_ERROR("invalid meta type");
   }
   
   ASSERT(meta);
   
   // valid variant found; now parse required and optional parameters
   if (!_parse_properties(req, obj, *meta, data))
      return false;
   
   // get parent blob object
   SceneNodeShape *parent = 
      dynamic_cast<SceneNodeShape*>(data.sceneGraph.top());
   ASSERT(parent);
   Shape *shape = parent->shape;
   ASSERT(shape);
   Blob *blob = dynamic_cast<Blob*>(shape);
   ASSERT(blob);
   
   blob->push_back(meta);
   
   return true;
}


#define DECLARE_META(prim) DECLARE_VARIANT(prim, meta)

DECLARE_META(ball);

bool MiltonJSONSceneLoader::_parse_spatialAccel(const JSONVariant &v, 
                                                JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("spatialAccel", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   JSONObjectConstIter typeIter = obj.find("type");
   
   if (typeIter == obj.end())
      PARSE_ERROR("unable to infer spatialAccel type");
   
   PARSE_CHECK_TYPE_ERROR("spatialAccel.type", "string", JSONString, 
                          typeIter->second);
   const std::string &type = boost::any_cast<JSONString>(*(typeIter->second));
   data << type << endl;
   
   SceneNodeShape *parent = 
      dynamic_cast<SceneNodeShape*>(data.sceneGraph.top());
   ASSERT(parent);
   Shape *shape = parent->shape;
   ASSERT(shape);
   PropertyMap *properties = dynamic_cast<PropertyMap*>(shape);
   ASSERT(properties);
   
   if (properties->contains("spatialAccel"))
      PARSE_ERROR("multiple spatialAccel definitions");
   
   KeyValueIDMap req;
   
   if (type == "naive") {
      properties->insert("spatialAccel", type);
   } else if (type == "kdTree") {
      properties->insert("spatialAccel", type);
      
      req["kdSplitPlaneType"] = "string";
      req["kdSplitAxisType"]  = "string";
      req["kdMinPrimitives"]  = "uint";
      req["kdMaxDepth"]       = "uint";
      req["kdNoThreads"]      = "uint";
      req["kdPostCompress"]   = "bool";
      req["kdCostTraversal"]  = "real_t";
      req["kdCostIntersect"]  = "real_t";
      req["kdEmptyBias"]      = "real_t";
   } else if (type == "dynamic") {
      
      NYI(); // TODO
      
   } else {
      PARSE_ERROR("invalid spatialAccel type");
   }
   
   // valid variant found; now parse required and optional parameters
   return _parse_properties(req, obj, *properties, data);
}

bool MiltonJSONSceneLoader::_parse_background(const JSONVariant &v, 
                                              JSONParseData &data) const
{
   PARSE_CHECK_TYPE_ERROR("background", "object", JSONObject, v);
   JSONObject const &obj = boost::any_cast<JSONObject>(*v);
   
   if (data.background != NULL)
      PARSE_ERROR("scene may contain at most one background element");
   
   if (obj.size() == 0)
      return true; // use default background
   
   JSONObjectConstIter emitterIter = obj.find("emitter");
   
   if (obj.size() > 1 || emitterIter == obj.end())
      PARSE_ERROR("background element must have exactly one child emitter element");
   
   data.background = new Material();
   
   if (_parse_emitter_helper(emitterIter->second, data, true)) {
      ASSERT(data.background);
      data.background->init();
      
      return true;
   }
   
   return false;
}

bool MiltonJSONSceneLoader::_parse_dummy(const JSONVariant &v, 
                                         JSONParseData &data) const
{
   return true;
}

bool MiltonJSONSceneLoader::_parse_real_t(const JSONVariant &v, 
                                          real_t &outVal) const
{
   const std::type_info &type = v->type();
   
   if (type == typeid(int))
      outVal = boost::any_cast<int>(*v);
   else if (type == typeid(double))
      outVal = static_cast<real_t>(boost::any_cast<double>(*v));
   else if (type == typeid(float))
      outVal = static_cast<real_t>(boost::any_cast<float>(*v));
   else
      return false;
   
   return true;
}

bool MiltonJSONSceneLoader::_parse_real_t_array(const JSONVariant &v, 
                                                real_t *outArray, 
                                                unsigned expectedLength, 
                                                JSONParseData &data) const
{
   ASSERT(outArray);
   ASSERT(expectedLength > 0);
   
   PARSE_CHECK_TYPE_ERROR("initializer", "array", JSONArray, v);
   JSONArray const &array = boost::any_cast<JSONArray>(*v);
   unsigned length = array.size();
   
   if (length != expectedLength)
      PARSE_ERROR("(incorrect length)");
   
   for(unsigned i = 0; i < length; ++i) {
      if (!_parse_real_t(array[i], outArray[i])) {
         PARSE_ERROR("(incorrect type)");
      }
   }
   
   return true;
}

template<typename T, unsigned E>
T *MiltonJSONSceneLoader::_parse_dynamic_plugin(JSONObject const &obj, 
                                                JSONParseData &data) const
{
   JSONObjectConstIter pathIter = obj.find("path");
   PARSE_CHECK_TYPE_ERROR("dynamic.path", "string", JSONString, 
                          pathIter->second);
   const std::string &path = boost::any_cast<JSONString>(*(pathIter->second));
   data << path << endl;
   
   SharedLibraryManager *manager = ResourceManager::manager;
   SharedLibrary *library = manager->load(path);
   
   if (NULL == library)
      PARSE_ERROR(path + std::string(": plugin failed to load: ") + manager->getLastError());
   
   void *plugin_type_symbol   = library->getSymbol("plugin_type");
   void *plugin_new_symbol    = library->getSymbol("plugin_new");
   //void *plugin_delete_symbol = library->getSymbol("plugin_delete");
   
   if (NULL == plugin_type_symbol)
      PARSE_ERROR(path + std::string(": invalid plugin; could not find plugin_type symbol ") + manager->getLastError());
   
   if (NULL == plugin_new_symbol)
      PARSE_ERROR(path + std::string(": invalid plugin; could not find plugin_new symbol ") + manager->getLastError());
   
   plugin_type_func_t plugin_type_func = (plugin_type_func_t) plugin_type_symbol;
   MILTON_PLUGIN_TYPE actualtype       = plugin_type_func();
   
   if (actualtype != E) {
      ostringstream s;
      s << path << ": dynamic plugin expected type '" << E << "' but found type '" 
        << actualtype << "' -- " <<  manager->getLastError();
      
      PARSE_ERROR(s.str());
   }
   
   plugin_new_func_t plugin_new_func = (plugin_new_func_t) plugin_new_symbol;
   void *plugin = plugin_new_func();
   
   if (plugin == NULL)
      PARSE_ERROR(path + std::string(": invalid plugin; plugin_new returned NULL; ") + manager->getLastError());
   
   return (T *)plugin;
}

}

