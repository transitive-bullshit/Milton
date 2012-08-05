/**<!-------------------------------------------------------------------->
   @file   InstancedShape.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Acts as a proxy for a shape which has been instanced
   <!-------------------------------------------------------------------->**/

#include "InstancedShape.h"
#include <SurfacePoint.h>
#include <Material.h>

namespace milton {

void InstancedShape::init() {
   // prevent infinite recursion if by some chance, an shape is self-instancing
   Shape *instancee = m_instancee;
   m_instancee = NULL;
   
   // ensure instancee has already been initted
   instancee->init();
   m_instancee = instancee;
   
   // setup AABB and transforms
   if (m_instancee->isTransformable()) {
      Transformable *trans = dynamic_cast<Transformable*>(m_instancee);
      m_objSpaceAABB    = trans->getObjSpaceAABB();
      
      // instanced shape's transform effectively 'undoes' the instancee's 
      // transform in addition to applying its own, without the instancee 
      // even knowing
      m_transToWorld    = m_transToWorld * trans->getTransToWorldInv();
      m_transToWorldInv = trans->getTransToWorld() * m_transToWorldInv;
   } else {
      m_objSpaceAABB = m_instancee->getAABB();
   }
   
   Transformable::init();
}

real_t InstancedShape::getIntersection(const Ray &ray, SurfacePoint &pt) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   const real_t t = m_instancee->getIntersection(Ray(P, D), pt);
   
   if (Ray::isValid(t))
      pt.shape = this;
   
   return t;
}

bool InstancedShape::intersects(const Ray &ray, real_t tMax) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   return m_instancee->intersects(Ray(P, D), tMax);
}

bool InstancedShape::hasNormal() const {
   return m_instancee->hasNormal();
}

void InstancedShape::preview() {
   Transformable::preview();
   
   m_instancee->preview();
}

void InstancedShape::getRandomPoint(SurfacePoint &pt) {
   m_instancee->getRandomPoint(pt);
   
   pt.shape = this;
}

Point3 InstancedShape::getPosition(const UV &uv) {
   const Point3 &position = m_instancee->getPosition(uv);
   
   return m_transToWorld * position;
}

real_t InstancedShape::_getSurfaceArea() {
   // TODO: this is probably incorrect for world-space surface area!!!
   
   return m_instancee->_getSurfaceArea();
}

void InstancedShape::initSurfacePoint(SurfacePoint &pt) const {
   const Point3 &position = pt.position;
   
   pt.position = m_transToWorldInv * position;
   m_instancee->initSurfacePoint(pt);
   pt.position = position;
   
   // TODO: prevent surface point from being initted twice
   ASSERT(m_material);
   m_material->initSurfacePoint(pt);
}

void InstancedShape::_getUV(SurfacePoint &pt) const {
   const Point3 &position = pt.position;
   
   pt.position = m_transToWorldInv * position;
   m_instancee->_getUV(pt);
   pt.position = position;
}

void InstancedShape::_getGeometricNormal(SurfacePoint &pt) const {
   const Point3 &position = pt.position;
   
   pt.position = m_transToWorldInv * position;
   m_instancee->_getGeometricNormal(pt);
   pt.position = position;
   
   _transformVector3ObjToWorld(pt.normalG, pt.normalG);
}

}

