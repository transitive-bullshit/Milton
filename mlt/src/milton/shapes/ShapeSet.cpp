/**<!-------------------------------------------------------------------->
   @file   ShapeSet.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a collection of shapes which all have the same transformation
   <!-------------------------------------------------------------------->**/

#include "ShapeSet.h"
#include <kdTreeAccel.h>
#include <NaiveSpatialAccel.h>
#include <ResourceManager.h>
#include <GL/gl.h>
#include <QtCore/QtCore>

namespace milton {

ShapeSet::~ShapeSet() {
   if (m_ownMemory) {
      FOREACH(PrimitiveListIter, m_primitives, iter) {
         Shape *shape = *iter;
         ASSERT(shape);
         
         safeDelete(shape);
      }
   }
   
   safeDelete(m_spatialAccel);
}

void ShapeSet::init() {
   init(true);
}

void ShapeSet::init(bool initChildren) {
   if (NULL == m_spatialAccel) {
      //ResourceManager::log.info << "ShapeSet::init " << 
      //   (initChildren ? "initChildren=true" : "initChildren=false") << 
      //   ", " << m_primitives.size() << endl;
      
      AABB aabb;
      
      if (initChildren) {
         ASSERT(m_material);
         FOREACH(PrimitiveListIter, m_primitives, iter) {
            Shape *s = *iter;
            ASSERT(s);
            
            ASSERT(s->getMaterial());
            //s->setMaterial(m_material);
            
            s->init();
            ASSERT(s->getAABB().isValid());
         }
      }
      
      const std::string &accelType = getValue<std::string>("spatialAccel", 
                                                           "kdTree");
      
      if (accelType == "kdTree") {
         m_spatialAccel = new kdTreeAccel();
      } else {
         ASSERT(accelType == "naive");
         
         m_spatialAccel = new NaiveSpatialAccel();
      }
      
      m_spatialAccel->setGeometry(reinterpret_cast<IntersectableList*>(&m_primitives));
      m_spatialAccel->inherit(*this);
      m_spatialAccel->init();
      
      m_objSpaceAABB = m_spatialAccel->getAABB();
      
      // transform AABB into parent coordinate system
      Transformable::init();
      
      //ResourceManager::log.info << "ShapeSet: " << m_aabb << endl;
   }
}

void ShapeSet::preview() {
   Transformable::preview();
   
   bool enableAccelPreview = 
      ResourceManager::getValue<bool>("enableAccelPreview", true);
   
   if (enableAccelPreview && m_spatialAccel)
      m_spatialAccel->preview();
   else m_aabb.preview();
   
   FOREACH(PrimitiveListIter, m_primitives, iter) {
      Shape *shape = *iter;
      ASSERT(shape);
      
      glPushMatrix();
      shape->preview();
      glPopMatrix();
   }
}

real_t ShapeSet::getIntersection(const Ray &ray, SurfacePoint &pt) {
   ASSERT(m_spatialAccel);
   
   // TODO: creating a new Ray here is rather inefficient...
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   return m_spatialAccel->getIntersection(Ray(P, D), pt);
}

bool ShapeSet::intersects(const Ray &ray, real_t tMax) {
   ASSERT(m_spatialAccel);
   
   // TODO: creating a new Ray here is rather inefficient...
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   return m_spatialAccel->intersects(Ray(P, D), tMax);
}

real_t ShapeSet::_getSurfaceArea() {
   real_t area = 0;
   
   FOREACH(PrimitiveListIter, m_primitives, iter) {
      Shape *shape = *iter;
      ASSERT(shape);
      
      area += shape->getSurfaceArea();
   }
   
   return area;
}

void ShapeSet::_getUV(SurfacePoint &pt) const {
   ASSERT(0 && "Shouldn't be here\n");
}

void ShapeSet::_getGeometricNormal(SurfacePoint &pt) const {
   ASSERT(0 && "Shouldn't be here\n");
}

}

