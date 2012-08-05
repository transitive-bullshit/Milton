/**<!-------------------------------------------------------------------->
   @file   Sensor.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of importance defined at a single point on a surface in 
   3-space (describing a sensor / camera).
   
      The reason Sensor subclasses BSDF is for convenience during generation 
   and evaluation of paths in bidirectional path tracing.  Importance at a 
   point in a given direction, We(x,wo), is broken into two parts, 
   We0(x), and We1(x,wo), as originally detailed by Veach and Guibas
   (see Eric Veach's thesis, section 8.3.2).  Scattering at a vertex can then 
   be uniformly viewed as evaluation / sampling of a BSDF, including initial, 
   fake 'scattering' at a sensor, which can be viewed as a special case 
   of scattering with no exitent vector (the exitent vector is thus 
   disregarded).
   
   @see Sensor::evaluate() or the Path class for more details.
   <!-------------------------------------------------------------------->**/

#include "Sensor.h"
#include "Material.h"
#include <Camera.h>

namespace milton {

void Sensor::init() {
   ASSERT(m_pt.shape);
   ASSERT(m_parent);
   
   m_camera = NULL;
   
   try {
      m_camera = dynamic_cast<Camera*>(m_pt.shape);
   } catch(std::bad_cast&) { }
   
   ASSERT(m_camera);
   BSDF::init();
}

bool Sensor::isSensor() {
   return (getImportance() != SpectralSampleSet::black());
}

SpectralSampleSet Sensor::getWe(const Vector3 &wo) {
   ASSERT(m_camera);
   
   return SpectralSampleSet::fill(wo == m_pt.normalS);
}

SpectralSampleSet Sensor::getImportance() {
   return SpectralSampleSet::identity();
}

SpectralSampleSet Sensor::getWe0() {
   return getImportance();
}

SpectralSampleSet Sensor::getWe1(const Vector3 &wo) {
   return getWe(wo) / getWe0();
}


// TODO: modify Camera interface / implementation to accomodate stochastic 
// sampling w/ deterministic Pinhole camera as a special-case
Event Sensor::sample(const Event &) {
   ASSERT(m_camera);
   
   /*const Ray &ray = m_camera->getWorldRay(
      Point2(m_pt.uv.u * m_camera->getWidth(), 
             m_pt.uv.v * m_camera->getHeight()));
   
   return Event(ray.direction, this);*/
   
   return Event(m_pt.normalS, this);
}

real_t Sensor::getPd(const Event &event) {
   const Vector3 &wo = event.getValue<const Vector3&>();
   
   if (m_wi == -m_pt.normalS)
      return 1;
   
   // else adjoint
   return (wo == m_pt.normalS);
}

SpectralSampleSet Sensor::evaluate(const Vector3 &wi, 
                                  const Vector3 &/* wo unused */)
{
   return getWe1(-wi);
   /*if (wi == -m_pt.normalS)
      return getWe1(-wi);
   
   // else adjoint
   return getWe1(wo);*/
}

}

