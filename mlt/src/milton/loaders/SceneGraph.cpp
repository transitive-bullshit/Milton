/**<!-------------------------------------------------------------------->
   @file   SceneGraph.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Bare-bones scene graph used during scene parsing
   <!-------------------------------------------------------------------->**/

#include "SceneGraph.h"
#include "MiltonJSONSceneLoader.h"
#include <milton.h>

#include <QtCore/QtCore>
#include <boost/any.hpp>
#include <typeinfo>
#include <iostream>
#include <fstream>

namespace milton {


bool SceneGraph::flatten(JSONParseData &data) {
   data << "flattening scenegraph" << endl;
   
   ASSERT(data.sceneGraph.size() == 1);
   ASSERT(data.instance == 0);
   
   { // initialize default material
      Material *defaultMaterial = new Material();
      defaultMaterial->init();
      data.activeMaterial = defaultMaterial;
      data.materials.push_back(defaultMaterial);
   }
   
   { // traverse scenegraph
      SceneNode *root = data.sceneGraph.pop();
      
      bool retVal = 
         flatten(root, Matrix4x4::identity(), Matrix4x4::identity(), data);
      
      ASSERT(data.instance == 0);
      safeDelete(root);
      
      return retVal;
   }
}

bool SceneGraph::flatten(SceneNode *node, 
                         Matrix4x4 transform, 
                         Matrix4x4 transformInv, 
                         JSONParseData &data)
{
   ASSERT(node);
   
   // pre-order traversal (visit current node before children)
   if (!node->flatten(transform, transformInv, data))
      return false;
   
   Material *activeMaterial = data.activeMaterial;
   ShapeSet *activeShapeSet = data.primitives;
   
   // traverse all children
   FOREACH(SceneNodeListIter, node->children, iter) {
      SceneNode *child = *iter;
      ASSERT(child);
      
      // recursively flatten current child
      if (!flatten(child, transform, transformInv, data))
         return false;
      
      // ensure active material is restored to current level
      data.activeMaterial = activeMaterial;
      data.primitives     = activeShapeSet;
   }
   
   return true;
}


bool SceneNodeTransform::flatten(Matrix4x4 &trans, 
                                 Matrix4x4 &transInv, 
                                 JSONParseData &data)
{
   trans *= getTransform();
   // TODO: deal with inverses
   
   return true;
}

bool SceneNodeMaterial::flatten(Matrix4x4 &trans, 
                                Matrix4x4 &transInv, 
                                JSONParseData &data)
{
   ASSERT(material);
   
   data.activeMaterial = material;
   data.materials.push_back(material);
   
   return true;
}

bool SceneNodeShape::flatten(Matrix4x4 &trans, 
                             Matrix4x4 &transInv, 
                             JSONParseData &data)
{
   Shape *current = shape;
   
   if (data.instance > 0)
      current = new InstancedShape(current);
   
   current->setMaterial(data.activeMaterial);
   
   Transformable *t = NULL;
   try {
      t = dynamic_cast<Transformable*>(current);
   } catch(std::bad_cast&) { }
   
   // if the current shape is transformable
   if (t) {
      const Matrix4x4 &newTrans = trans * t->getTransToWorld();
      t->setTransToWorld(newTrans);
      // TODO: use transInv instead of trans.getInverse()
   }
   
   ShapeSet *shapeSet = NULL;
   try {
      shapeSet = dynamic_cast<ShapeSet*>(current);
   } catch(std::bad_cast&) { }
   
   if (shapeSet)
      data.primitives = shapeSet;
   else
      data.primitives->push_back(current);
   
   return true;
}

bool SceneNodeInstance::flatten(Matrix4x4 &trans, 
                                Matrix4x4 &transInv, 
                                JSONParseData &data)
{
   InstancedNodeMapIter instanceeIter = data.instancedNodes.find(instancee);
   if (instanceeIter == data.instancedNodes.end()) {
      data << "error: instanced node '" << instancee << "' not found" << endl;
      
      return false;
   }
   
   SceneNode *node = instanceeIter->second;
   
   if (NULL == node) {
      data << "error: instanced node '" << instancee << "' not found" << endl;
      
      return false;
   }
   
   if (node->type != instanceeType) {
      data << "error: instanced node '" << instancee 
         << "' type mismatch with expected type" << endl;
      
      return false;
   }
   
   Material *activeMaterial = data.activeMaterial;
   ShapeSet *activeShapeSet = data.primitives;
   
   // record the fact that we're instanced
   ++data.instance;
   
   data.sceneGraph.flatten(node, trans, transInv, data);
   
   // pop this instance off of the implicit instance stack
   --data.instance;
   
   // ensure active material is restored to current level
   data.activeMaterial = activeMaterial;
   data.primitives     = activeShapeSet;
   
   return true;
}

}

