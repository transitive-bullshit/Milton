/**<!-------------------------------------------------------------------->
   @file   DielectricBSDF.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Wavelength-dependent, thin dielectric BSDF defined at a single point on 
   a surface in 3-space. The opacity input controls the transmittance versus 
   reflectance of different wavelengths. A pure specular mirror may be obtained
   by creating a dielectric BSDF with zero opacity. Conversely, a purely 
   transparent surface may be obtained by creating a dielectric with full 
   opacity. Note that a completely transparent surface will still reflect some 
   light (in proportion to Fresnel's Laws) because refraction is undefined past 
   a critical angle when light is traveling between a lighter and denser 
   medium. When this happens, some light is reflected regardless of the opacity 
   parameter, and this is typically known as total internal reflection.
   
   @param
      opacity - SpectralSampleSet which specifies the transmittance of the 
   material
   
   @param
      ks      - SpectralSampleSet which scales / attenuates the overall 
   reflectance of the material (specular albedo)
   <!-------------------------------------------------------------------->**/

#include "DielectricBSDF.h"
#include <IndexOfRefraction.h>
#include <Material.h>
#include <Fresnel.h>
#include <Random.h>

namespace milton {

struct DielectricMetaData {
   bool     reflect;
   unsigned index;
   
   inline DielectricMetaData(bool reflect_, 
                             unsigned index_)
      : reflect(reflect_), index(index_)
   { }
   
   inline DielectricMetaData()
   { }
};

void DielectricBSDF::init() {
   m_ior = m_parent->getSpectralSampleSet("ior", IndexOfRefraction::AIR, m_pt);
   
   m_transparency    = m_parent->getSpectralSampleSet("transparency", 1.0, m_pt);
   m_avgTransparency = m_transparency.getAverage();
}

Event DielectricBSDF::sample(const Event &e) {
   const Vector3 &N  = 
      (m_pt.normal.dot(m_wi) > 0 ? -m_pt.normalS : m_pt.normalS);
   
   // simulate dispersion by allowing the internal index of refraction to vary 
   // per-wavelength; as an optimization to produce lower-variance paths which 
   // have a coherent ior index throughout, we allow the caller to set a 
   // preferred ior index in the underlying SurfacePoint (any non-zero value), 
   // s.t. the caller can set one preferred wavelength per-path
   unsigned index = 
      (m_pt.iorIndex > 0 ? 
       m_pt.iorIndex - 1 : 
       Random::sampleInt(0, m_ior.getN()));
   
   // attempt to preserve scattering mode of event passed in
   if (!e.getMetadata().empty()) {
      try {
         const DielectricMetaData &data = 
            e.getMetadata<const DielectricMetaData&>();
         
         if (data.reflect)
            return Event(m_wi.reflectVector(N), this, DielectricMetaData(true, index));
         
         index = data.index;
         const real_t ior  = m_ior[index].value;
         const Vector3 &wt = m_wi.refractVector(m_pt.normalS, m_pt.ior1, ior);
         
         return Event(wt, this, DielectricMetaData(false, index));
      } catch(boost::bad_any_cast &) { }
   }
   
   const real_t ior  = m_ior[index].value;
   const Vector3 &wt = m_wi.refractVector(m_pt.normalS, m_pt.ior1, ior);
   
   if (wt != Vector3::zero()) {
      const real_t Fr = Fresnel::getFr(m_wi, m_pt.normalS, m_pt.ior1, ior);
      ASSERT(Fr >= 0 && Fr <= 1);
      
      // attenuate normal Fresnel transmission coefficient, Fs, by the average 
      // transparency of the surface
      const real_t Fs = m_avgTransparency * (1 - Fr);
      ASSERT(Fs >= 0 && Fs <= 1);
      
      // refract with probability Fs
      if (Random::sample() < Fs)
         return Event(wt, this, DielectricMetaData(false, index));
   }
   
   return Event(m_wi.reflectVector(N), this, DielectricMetaData(true, index));
}

real_t DielectricBSDF::getPd(const Event &event) {
   const Vector3 &wo = event.getValue<const Vector3&>();
   const Vector3 &N  = 
      (m_pt.normal.dot(m_wi) > 0 ? -m_pt.normalS : m_pt.normalS);
   
   if (wo.dot(N) < 0) {
      //const Vector3 &wt = m_wi.refractVector(m_pt.normalS, m_pt.ior1, m_pt.ior2);
      
      if (m_pt.iorIndex > 0) {
         const Vector3 &wt = m_wi.refractVector(m_pt.normalS, m_pt.ior1, m_ior[m_pt.iorIndex - 1].value);
         
         return (wo == wt);
      }
      
      real_t n = 0;
      for(unsigned i = m_ior.getN(); i--;) {
         const Vector3 &wt = m_wi.refractVector(m_pt.normalS, m_pt.ior1, m_ior[i].value);
         
         n += (wo == wt);
      }
      
      return n / m_ior.getN();
      //return (wo == wt ? 1 : 0);
   }
   
   return (wo == m_wi.reflectVector(N) ? 1 : 0);
}

SpectralSampleSet DielectricBSDF::evaluate(const Vector3 &wi, const Vector3 &wo) {
   const SpectralSampleSet &ks = 
      m_parent->getSpectralSampleSet("ks", SpectralSampleSet::fill(1.0), m_pt);
   const Vector3 &N   = 
      (m_pt.normal.dot(wi) > 0 ? -m_pt.normalS : m_pt.normalS);
   
   if (wo.dot(N) < 0) {
      SpectralSampleSet s(m_ior);
      for(unsigned i = m_ior.getN(); i--;) {
         const Vector3 &wt = wi.refractVector(m_pt.normalS, m_pt.ior1, m_ior[i].value);
         
         if (m_pt.iorIndex > 0) {
            s[i].value = (i == m_pt.iorIndex - 1 && wo == wt);
         } else {
            s[i].value = (wo == wt);
         }
      }
      
      return ks * s;
      //return ks * (wo == wt);
   }
   
   return ks * (wo == wi.reflectVector(N));
}

}

