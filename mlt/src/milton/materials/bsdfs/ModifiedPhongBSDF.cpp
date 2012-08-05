/**<!-------------------------------------------------------------------->
   @file   ModifiedPhongBSDF.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Physically-correct modified phong model for glossy/specular surfaces, 
   defined at a single point on a surface in 3-space. The modified phong 
   model has two inputs: Kd, and Ks in [0, 1] subject to Kd + Ks <= 1, 
   where Kd and Ks represent the diffuse and specular reflectivity of the 
   surface respectively (fraction of incoming energy that is reflected 
   diffusely/specularly). A third input, n, represents the specular 
   shininess of the surface, where higher values of n correspond to tighter /
   sharper specular highlights, and lower values of n correspond to wider / 
   glossier highlights.
   
   @note For more information, please see: 
      E. Lafortune and Y. Willems. Using the modified Phong reflectance 
   model for physically based rendering. Technical Report CW197, Dept of 
   Computer Science, K.U. Leuven, 1994.
   
   @param
      kd - SpectralSampleSet which scales / attenuates the overall reflectance 
   of the material (diffuse albedo)
   
   @param
      ks - SpectralSampleSet which scales / attenuates the overall reflectance 
   of the material (specular albedo)
   
   @param
      n  - Wavelength-dependent shininess exponent
   
   @note Also known as modified Blinn-Phong model
   <!-------------------------------------------------------------------->**/

#include "ModifiedPhongBSDF.h"
#include <Material.h>
#include <Random.h>

namespace milton {

enum {
   MODIFIED_PHONG_EVENT_ABSORPTION = 0, 
   MODIFIED_PHONG_EVENT_DIFFUSE, 
   MODIFIED_PHONG_EVENT_SPECULAR, 
};

void ModifiedPhongBSDF::init() {
   BSDF::init();
   
   if (m_parent->contains("ks")) {
      m_ks = m_parent->getSpectralSampleSet("ks", m_pt);
      m_kd = m_parent->getSpectralSampleSet("kd", SpectralSampleSet::black(), m_pt);
   } else if (m_parent->contains("kd")) {
      m_kd = m_parent->getSpectralSampleSet("kd", m_pt);
      m_ks = m_parent->getSpectralSampleSet("ks", SpectralSampleSet::black(), m_pt);
   } else {
      m_kd = m_parent->getSpectralSampleSet("kd", SpectralSampleSet::fill(.5), m_pt);
      m_ks = m_parent->getSpectralSampleSet("ks", SpectralSampleSet::fill(.5), m_pt);
   }
   
      /*unsigned k = m_ks.getMaxFrequency();
      if (m_ks[k] > 0) {
         real_t ratio = (1 - m_ks[k]) / m_ks[k];
         
         for(unsigned i = m_kd.N; i--;)
            m_kd[i] = m_ks[i] * ratio;
      }*/
   //m_ks = SpectralSampleSet::black(); // TODO: temporary
   //m_kd = SpectralSampleSet::black(); // TODO: temporary
   //cerr << "temporary\n\n" << endl;
   
   m_n = m_parent->getSpectralSampleSet("n" , SpectralSampleSet::fill(1.0), m_pt);
   
   ASSERT(m_kd + m_ks <= SpectralSampleSet::identity());
   ASSERT(m_n >= SpectralSampleSet::black());
   
   m_kda = m_kd.getAverage();
   m_ksa = m_ks.getAverage();
   m_na  = m_n.getAverage();
}

Event ModifiedPhongBSDF::sample(const Event &) {
   ASSERT(m_n >= SpectralSampleSet::fill(0));
   
   const real_t p0 = Random::sample() - EPSILON;
   unsigned index  = MODIFIED_PHONG_EVENT_ABSORPTION;
   
   if (p0 <= m_kda) {               // diffuse reflection
      index = MODIFIED_PHONG_EVENT_DIFFUSE;
      
      const Vector3 &wo = Vector3::cosRandom(m_pt.normalS);
      
      return Event(wo, this, index);
   } else if (p0 <= m_kda + m_ksa) { // specular reflection
      index = MODIFIED_PHONG_EVENT_SPECULAR;
      
      const real_t alpha = acos(pow(Random::sample(), 
                                    1.0 / (m_na + 1)));
      const real_t phi   = 2.0 * M_PI * Random::sample();
      
      // perfect specular direction
      const Vector3 &R = m_wi.reflectVector(m_pt.normalS);
      Vector3 r = R;
      Vector3 u, v;
      
      if (ABS(m_pt.normal.dot(r)) < .95) {
         // construct orthonormal basis with r and v in the plane shared by 
         // r and the local surface normal
         u = r.cross(m_pt.normalS).getNormalized();
         v = u.cross(r);
      } else { // r approximately collinear with normal
         r.getOrthonormalBasis(v, u);
      }
      
      const Vector3 &w  = Vector3::fromSpherical(alpha, phi);
      const Vector3 &wo = u * w[0] + r * w[1] + v * w[2];
      
      if (m_pt.normal.dot(wo) > 0)
         return Event(wo, this, index);
      
      // randomly sampled vector on opposite side of hemisphere
      return Event(R, this, index);
   }
   
   // absorbed
   return Event(Vector3(), this, index);
}

real_t ModifiedPhongBSDF::getPd(const Event &event) {
   const unsigned index = event.getMetadata<unsigned>();
   real_t pdf = 1;
   
   if (index == MODIFIED_PHONG_EVENT_ABSORPTION) {
      pdf = (1 - (m_kda + m_ksa));
      ASSERT(pdf > 0);
      
      return pdf;
   } else {
      const Vector3 &wo = event.getValue<const Vector3&>();
      const real_t cosA = m_pt.normal.dot(wo);
      if (cosA <= 0)
         return 0;
      
      if (index == MODIFIED_PHONG_EVENT_SPECULAR) { // specular reflection
         const Vector3 &r  = m_wi.reflectVector(m_pt.normalS);
         const real_t cosA = ABS(r.dot(wo));
         ASSERT(m_ksa > 0);
         ASSERT(r.isUnit());
         
         pdf = MIN(1.0, (m_na + 1) / (2 * M_PI) * pow(cosA, m_na));
      } else {        // diffuse reflection
         ASSERT(index == MODIFIED_PHONG_EVENT_DIFFUSE);
         ASSERT(m_kda > 0);
         
         pdf = (ABS(m_pt.normal.dot(wo)) / M_PI);
         ASSERT(pdf > 0);
      }
      
      return pdf / cosA;
   }
}

SpectralSampleSet ModifiedPhongBSDF::evaluate(const Vector3 &wi, const Vector3 &wo) {
   if (m_pt.normal.dot(wo) <= 0)
      return SpectralSampleSet::black();
   
   const Vector3 &r = wi.reflectVector(m_pt.normalS);
   const real_t   a = CLAMP(r.dot(wo), 0, 1);
   SpectralSampleSet a_n(m_n);
   
   for(unsigned i = m_n.getN(); i--;)
      a_n[i].value = pow(a, m_n[i].value);
   
   const SpectralSampleSet &fs_s = 
      (m_ksa > 0 ? ((m_n + SpectralSampleSet::fill(2)) / (2 * M_PI * m_ksa)) * a_n : 
       SpectralSampleSet::black());
   const real_t fs_d = (m_kda > 0 ? 1.0 / (M_PI * m_kda) : 0);
   
   return m_kd * fs_d + m_ks * fs_s;
}

}

