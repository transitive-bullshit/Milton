/**<!-------------------------------------------------------------------->
   @file   EnvironmentMap.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Environment map using latitude-longitude format.  An environment map 
   roughly models distant illumination surrounding a scene by conceptually 
   surrounding the scene with a spherical emitter that emits light inwards 
   according to a distribution defined by a 2D 'environment' texture map that 
   is projected onto the surface of the sphere.  Note the term HDR environment 
   map is thrown around a lot; the only difference is that the underlying 
   environment texture is stored in some type of high dynamic range format, 
   such as OpenEXR or HDR.
   <!-------------------------------------------------------------------->**/

#include "EnvironmentMap.h"
#include <ResourceManager.h>
#include <filters.h>
#include <QtCore/QtCore>

namespace milton {

void EnvironmentMap::init() {
   ASSERT(m_parent);
   Emitter::init();
   
   m_filter = m_parent->getFilter();
   
   ASSERT(m_parent->contains("path"));
   const std::string &filename = m_parent->getValue<std::string>("path");
   m_image = ResourceManager::getImage(filename);
   
   if (!m_image) {
      cerr << "failed to load environment map image '" << filename << "'" 
           << endl << endl;
      ASSERT(0);
   }
}

void EnvironmentMap::preview(Shape *shape)
{ }

SpectralSampleSet EnvironmentMap::getLe(const Vector3 &w) {
   ASSERT(m_filter);
   ASSERT(m_image);
   
   const UV &uv = getUV(w);
   
   const real_t repeatU  = m_parent->getValue<real_t>("repeatU", 1);
   const real_t repeatV  = m_parent->getValue<real_t>("repeatV", 1);
   const unsigned width  = m_image->getWidth();
   const unsigned height = m_image->getHeight();
   
   const real_t s = uv.u * repeatU;
   const real_t t = uv.v * repeatV;
   const real_t x = CLAMP((s - floor(s)) * width,  0, width  - 1);
   const real_t y = CLAMP((t - floor(t)) * height, 0, height - 1);
   
   return SpectralSampleSet(m_filter->apply(m_image.get(), x, y));
}

Vector2 EnvironmentMap::getSphericalCoords(const Vector3 &w) {
   const real_t rho   = sqrt(w[0] * w[0] + w[2] * w[2]);
   const real_t mag   = w.getMagnitude();
   
   const real_t theta = (rho < ABS(w[1]) ? 
                         acos(rho / mag) * (w[1] >= 0 ? 1 : -1) : 
                         asin(w[1] / mag));
   const real_t phi   = (w[0] == 0 && w[2] == 0) ? 0 : atan2(w[0], w[2]);
   
   return Vector2(theta, phi);
}

Vector2 EnvironmentMap::getSphericalCoords(const UV &uv) {
   return Vector2(-M_PI * uv.v - 0.5, -2 * M_PI * uv.u - 0.5);
}

UV EnvironmentMap::getUV(const Vector3 &w) {
   return getUV(getSphericalCoords(w));
}

UV EnvironmentMap::getUV(const Vector2 &coords) {
   return UV(coords[1] / (-2 * M_PI) + 0.5, coords[0] / -M_PI + 0.5);
}

Vector3 EnvironmentMap::getDirection(const UV &uv) {
   const Vector2 &coords = getSphericalCoords(uv);
   
   return Vector3(sin(coords[1]) * cos(coords[0]), 
                  sin(coords[0]), 
                  cos(coords[1]) * cos(coords[0]));
}

}

