/**<!-------------------------------------------------------------------->
   @file   DiffuseBSDF.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Ideal diffuse BSDF (lambertian) defined at a single point on a 
   surface in 3-space
   
   @param
      kd - SpectralSampleSet which scales / attenuates the overall reflectance 
   of the material (diffuse albedo)
   <!-------------------------------------------------------------------->**/

#include "DiffuseBSDF.h"
#include <Material.h>
#include <Random.h>

namespace milton {

Event DiffuseBSDF::sample(const Event &) {
   if (m_pt.normal.dot(m_wi) > 0)
      return Event(Vector3(), this); // absorbed
   
   const Vector3 &wo = Vector3::cosRandom(m_pt.normalS);
   
   return Event(wo, this);
}

real_t DiffuseBSDF::getPd(const Event &event) {
   if (m_pt.normal.dot(m_wi) > 0)
      return 0;
   
   const Vector3 &wo = event.getValue<const Vector3&>();
   const real_t cosA = m_pt.normal.dot(wo);
   
   return (cosA > 0 ? 1.0 / M_PI : 0);
}

SpectralSampleSet DiffuseBSDF::evaluate(const Vector3 &wi, const Vector3 &wo) {
   if (m_pt.normal.dot(wi) >= 0 || m_pt.normal.dot(wo) <= 0) // one-sided BRDF
      return SpectralSampleSet::black();
   
   const SpectralSampleSet &kd = 
      m_parent->getSpectralSampleSet("kd", SpectralSampleSet::fill(0.5), m_pt);
   
   return kd / M_PI;
}

}

