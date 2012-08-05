/**<!-------------------------------------------------------------------->
   @file   AggregateBSDF.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Linear combination of different BSDF implementations, where the 
   coefficients are determined either a priori (inherent property described 
   in scenefile) or via a texture map lookup
   <!-------------------------------------------------------------------->**/

#include "AggregateBSDF.h"
#include "Material.h"
#include <Random.h>

namespace milton {

AggregateBSDF::~AggregateBSDF() {
   FOREACH(BSDFListIter, m_bsdfs, iter) {
      safeDelete(iter->bsdf);
   }
}

void AggregateBSDF::init() {
   BSDF::init();
   
   if (m_bsdfs.size() <= 0) {
      ASSERT(m_parent);
      ASSERT(m_parent->contains("bsdfs"));
      
      m_bsdfs = m_parent->getValue<BSDFList>("bsdfs");
   }
   
   // select active bsdf according to aggregate pdfs (cdf)
   const real_t x = Random::sample() - EPSILON;
   unsigned index = 0;
   real_t sum = 0;
   
   for(unsigned i = m_bsdfs.size(); i--;) {
      // sanity checks
      ASSERT(m_bsdfs[i].pdf >= 0 && m_bsdfs[i].pdf <= 1);
      ASSERT(m_bsdfs[i].bsdf && m_bsdfs[i].bsdf != this);
      sum += m_bsdfs[i].pdf;
      
      if (x <= sum) {
         index = i;
         break;
      }
   }
   
   ASSERT(sum >= 0 && sum <= 1 + EPSILON);
   ASSERT(m_bsdf < m_bsdfs.size());
}

void AggregateBSDF::setWi(const Vector3 &wi) {
   FOREACH(BSDFListIter, m_bsdfs, iter) {
      ASSERT(iter->bsdf);
      iter->bsdf->setWi(wi);
   }
}

Event AggregateBSDF::sample() {
   m_bsdfs[m_bsdf].bsdf->setWi(m_wi);
   return m_bsdfs[m_bsdf].bsdf->sample();
}

real_t AggregateBSDF::getPd(const Event &event) {
   //const unsigned index = event.getMetadata<unsigned>();
   // TODO: correct?
   m_bsdfs[m_bsdf].bsdf->setWi(m_wi);
   return m_bsdfs[m_bsdf].bsdf->getPd(event) / m_bsdfs[m_bsdf].pdf;
}

SpectralSampleSet AggregateBSDF::evaluate(const Vector3 &wi, const Vector3 &wo) {
   return m_bsdfs[m_bsdf].bsdf->evaluate(wi, wo);
}

bool AggregateBSDF::isSpecular(Event &event) const {
   return m_bsdfs[m_bsdf].bsdf->isSpecular(event);
}

}

