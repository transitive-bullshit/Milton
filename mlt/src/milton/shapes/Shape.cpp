/**<!-------------------------------------------------------------------->
   @file   Shape.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract representation of a shape in 3-space
   <!-------------------------------------------------------------------->**/

#include "Shape.h"
#include <SurfacePoint.h>
#include <Material.h>
#include <GL/gl.h>

namespace milton {

void Shape::initSurfacePoint(SurfacePoint &pt) const {
   ASSERT(m_material);
   
   _getUV(pt);
   
   if ((pt.normal.hasNormal = hasNormal()))
      _getGeometricNormal(pt);
   
   // fill in material properties (bsdf, emitter, normalS)
   m_material->initSurfacePoint(pt);
}

void Shape::init() {
   ASSERT(m_aabb.isValid());
   ASSERT(m_material);
   
   m_surfaceArea = _getSurfaceArea();
}

void Shape::preview() {
   ASSERT(m_material);
   m_material->preview(this);
}

bool Shape::hasNormal() const {
   return true;
}

void Shape::getRandomPoint(SurfacePoint &pt) {
   pt.uv.u = Random::sample(0, 1);
   pt.uv.v = Random::sample(0, 1);
   
   getPoint(pt, UV(Random::sample(0, 1), Random::sample(0, 1)));
}

void Shape::getPoint(SurfacePoint &pt, const UV &uv) {
   ASSERT(m_material);
   
   pt.shape    = this;
   pt.position = getPosition(uv);
   pt.uv       = uv;
   
   if ((pt.normal.hasNormal = hasNormal()))
      _getGeometricNormal(pt);
   
   // fill in material properties (bsdf, emitter, normalS)
   m_material->initSurfacePoint(pt);
}

Point3 Shape::getPosition(const UV &uv) {
   return Point3();
}

real_t Shape::getSurfaceArea() {
   return m_surfaceArea;
}

real_t Shape::_getSurfaceArea() {
   return 0;
}

}

