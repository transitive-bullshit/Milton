/**<!-------------------------------------------------------------------->
   @file   Blob.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      Blobby isosurface composed of a set of MetaObjects, all of which 
   combine to form a scalar field whose contour at this blob's threshold 
   value defines the surface. Upon initialization (Blob::init), the implicit 
   surface is polygonized into triangles using a variant of the well-known 
   Marching Cubes algorithm.
   
   @see also MetaObject
   <!-------------------------------------------------------------------->**/

#include "Blob.h"
#include "Mesh.h"
#include "MarchingCubes.h"

#include <SurfacePoint.h>

namespace milton {

Blob::~Blob() {
   FOREACH(MetaObjectListIter, m_metaObjects, iter) {
      MetaObject *meta = (*iter);
      
      safeDelete(meta);
   }
   
   safeDelete(m_mesh);
}

void Blob::init() {
   if (NULL == m_mesh) {
      MetaObject::init();
      
      m_threshold = getValue<real_t>("threshold", m_threshold);
      const Vector3 &resolution = getValue<Vector3>("resolution", 
                                                    Vector3(128, 128, 128));
      
      // no triangles will be generated with a threshold of zero because 
      // the value of the field will never be "on the other side" of zero
      // (it's bounded to be zero when there is no charge)
      // note: now handled in MarchingCubes algorithm instead
      //if (m_threshold == 0)
      //   m_threshold = EPSILON;
      
      m_aabb = AABB();
      m_aabb = AABB();
      
      // initialize metaObjects
      FOREACH(MetaObjectListIter, m_metaObjects, iter) {
         MetaObject *meta = (*iter);
         ASSERT(meta);
         
         meta->init();
         
         // keep track of aggregate AABB now because it will be used by 
         // the marching cubes algorithm
         m_aabb.add(meta->getAABB());
      }
      
      MarchingCubes marchingCubes(resolution);
      
      // initialize mesh
      m_mesh = marchingCubes.polygonize(*this, m_threshold);
      ASSERT(m_mesh);
      
      m_mesh->setMaterial(m_material);
      m_mesh->inherit(*this);
      m_mesh->init();
      
      if (m_mesh->getNoTriangles() <= 0 || m_mesh->getNoVertices() <= 0) {
         cerr << "Warning: blobby metaobject produced invalid mesh (no triangles)" 
              << endl;
      }
      
      // overwrite original, conservative AABB with tighter AABB
      m_objSpaceAABB = m_mesh->getAABB();
      
      // transform AABB into parent coordinate system
      Transformable::init();
   }
}

void Blob::preview() {
   ASSERT(m_mesh);
   Transformable::preview();
   
   m_mesh->preview();
}

real_t Blob::getIntersection(const Ray &ray, SurfacePoint &pt) {
   ASSERT(m_mesh);
   
   // TODO: creating a new Ray here is rather inefficient...
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   return m_mesh->getIntersection(Ray(P, D), pt);
}

bool Blob::intersects(const Ray &ray, real_t tMax) {
   ASSERT(m_mesh);
   
   // TODO: creating a new Ray here is rather inefficient...
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   return m_mesh->intersects(Ray(P, D), tMax);
}

real_t Blob::evaluate(const Point3 &pt) const {
   real_t charge = 0;
   
   FOREACH(MetaObjectListConstIter, m_metaObjects, iter) {
      const MetaObject *meta = (*iter);
      
      const real_t curCharge = meta->evaluate(pt);
      if (!meta->isNegative())
         charge += curCharge;
      else charge -= curCharge;
   }
   
   return charge;
}

AABB Blob::getAABB() const {
   return m_aabb;
}

void Blob::_getUV(SurfacePoint &pt) const {
   NYI(); // TODO / or just defer to Mesh (current)
}

void Blob::_getGeometricNormal(SurfacePoint &pt) const {
   NYI(); // TODO / or just defer to Mesh (current)
}

}

