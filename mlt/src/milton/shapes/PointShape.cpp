/**<!-------------------------------------------------------------------->
   @file   PointShape.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a single point in 3-space (used to represent point 
   lights and pinhole cameras)
   <!-------------------------------------------------------------------->**/

#include "PointShape.h"
#include "SurfacePoint.h"
#include <Random.h>

#include <GL/gl.h>

namespace milton {

void PointShape::init() {
   m_objSpaceAABB.min = Vector3((m_transToWorld * Point3()).data);
   m_objSpaceAABB.max = m_objSpaceAABB.min;
   
   Transformable::init();
}

real_t PointShape::getIntersection(const Ray &ray, SurfacePoint &pt) {
   return INFINITY;
}

bool PointShape::intersects(const Ray &ray, real_t tMax) {
   return false;
}

void PointShape::_getUV(SurfacePoint &pt) const {
   pt.uv.u = 0;
   pt.uv.v = 0;
}

void PointShape::_getGeometricNormal(SurfacePoint &pt) const {
   ASSERT(0 && "should not be here; PointShapes do not have normals");
}

void PointShape::preview() {
   Transformable::preview();
   
   glBegin(GL_POINTS);
   
   glVertex3f(0, 0, 0);
   
   glEnd();
}

bool PointShape::hasNormal() const {
   return false;
}

}

