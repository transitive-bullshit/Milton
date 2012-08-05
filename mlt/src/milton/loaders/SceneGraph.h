/**<!-------------------------------------------------------------------->
   @class  SceneGraph
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Bare-bones scene graph used during scene parsing
   <!-------------------------------------------------------------------->**/

#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <materials/Material.h>
#include <shapes/Shape.h>
#include <stack>

namespace milton {

struct JSONParseData;
struct SceneNode;

DECLARE_STL_TYPEDEF(std::vector<SceneNode*>, SceneNodeList);
typedef std::stack <SceneNode*> SceneNodeStack;

enum SceneNodeType {
   SCENE_NODE_TRANSFORM = (1 << 0), 
   SCENE_NODE_MATERIAL  = (1 << 1), 
   SCENE_NODE_SHAPE     = (1 << 2), 
   SCENE_NODE_INSTANCE  = (1 << 3)
};

struct MILTON_DLL_EXPORT SceneNode {
   SceneNodeList children;
   SceneNodeType type;
   std::string   name;
   
   inline explicit SceneNode(const SceneNodeType &type_)
      : type(type_)
   { }
   
   virtual bool flatten(Matrix4x4 &transform, 
                        Matrix4x4 &transformInv, 
                        JSONParseData &data) = 0;
   
   virtual ~SceneNode() {
      FOREACH(SceneNodeListIter, children, iter) {
         safeDelete(*iter);
      }
   }
};

struct MILTON_DLL_EXPORT SceneNodeTransform : public SceneNode {
   Matrix4x4     transform;
   Matrix4x4     translation;
   Matrix4x4     rotation;
   Matrix4x4     scale;
   
   SceneNodeTransform() 
      : SceneNode(SCENE_NODE_TRANSFORM), 
        transform(Matrix4x4::identity()), translation(Matrix4x4::identity()), 
        rotation(Matrix4x4::identity()),  scale(Matrix4x4::identity())
   { }
   
   virtual bool flatten(Matrix4x4 &trans, 
                        Matrix4x4 &transInv, 
                        JSONParseData &data);
   
   inline Matrix4x4 getTransform() {
      return transform * translation * rotation * scale;
   }
};

struct MILTON_DLL_EXPORT SceneNodeMaterial : public SceneNode {
   Material     *material;
   
   SceneNodeMaterial(Material *material_ = NULL) 
      : SceneNode(SCENE_NODE_MATERIAL), material(material_)
   { }
   
   virtual bool flatten(Matrix4x4 &transform, 
                        Matrix4x4 &transformInv, 
                        JSONParseData &data);
};

struct MILTON_DLL_EXPORT SceneNodeShape : public SceneNode {
   Shape        *shape;
   
   SceneNodeShape(Shape *shape_ = NULL)
      : SceneNode(SCENE_NODE_SHAPE), shape(shape_)
   { }
   
   virtual bool flatten(Matrix4x4 &transform, 
                        Matrix4x4 &transformInv, 
                        JSONParseData &data);
};

struct MILTON_DLL_EXPORT SceneNodeInstance : public SceneNode {
   std::string   instancee;
   SceneNodeType instanceeType;
   
   SceneNodeInstance(const std::string   &instancee_, 
                     const SceneNodeType &instanceeType_)
      : SceneNode(SCENE_NODE_INSTANCE), instancee(instancee_), 
        instanceeType(instanceeType_)
   { }
   
   virtual bool flatten(Matrix4x4 &transform, 
                        Matrix4x4 &transformInv, 
                        JSONParseData   &data);
};

struct MILTON_DLL_EXPORT SceneGraph {
   SceneNodeStack sceneGraph;
   
   virtual ~SceneGraph() {
      reset();
   }
   
   inline void beginNode(SceneNode *node) {
      push(node);
   }
   
   /// removes top node from stack and adds it to children of new top of stack
   inline void endNode() {
      SceneNode *node = pop();
      
      top()->children.push_back(node);
   }
   
   /// flattens this scenegraph into the given output data
   virtual bool flatten(JSONParseData &outData);
   
   /// adds another stack frame
   inline void push(SceneNode *node) {
      ASSERT(node);
      sceneGraph.push(node);
   }
   
   /// @returns the top stack frame
   inline SceneNode *top() {
      ASSERT(!empty());
      SceneNode *t = sceneGraph.top();
      
      ASSERT(t);
      return t;
   }
   
   /// @returns and removes the top stack frame
   inline SceneNode *pop() {
      ASSERT(!empty());
      SceneNode *t = top();
      sceneGraph.pop();
      
      return t;
   }
   
   /// @returns whether or not this scenegraph is currently empty
   inline bool empty() {
      return sceneGraph.empty();
   }
   
   /// @returns the number of frames in the stack
   inline unsigned size() {
      return sceneGraph.size();
   }
   
   /// clears the stack and frees its resources
   void reset() {
      while(!empty()) {
         SceneNode *t = top();
         
         safeDelete(t);
         pop();
      }
      
      ASSERT(sceneGraph.empty());
   }
   
   virtual bool flatten(SceneNode *node, Matrix4x4 transform, 
                        Matrix4x4 transformInv, 
                        JSONParseData &data);
};

}

#endif // SCENE_GRAPH_H_

