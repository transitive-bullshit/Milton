/**<!-------------------------------------------------------------------->
   @file   EmitterSampler.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Sampler which selects a point on an emitter (light source) in the scene 
   with probability proportional to radiant exitance.
   <!-------------------------------------------------------------------->**/

#include "EmitterSampler.h"
#include <SurfacePoint.h>
#include <ShapeSet.h>
#include <Material.h>
#include <Scene.h>

namespace milton {

void EmitterSampler::init(ShapeSet *lights) {
   ASSERT(lights);
   
   m_lights = lights;
   m_n      = m_lights->size();
   
   if (m_n <= 0)
      return;
   
   safeDeleteArray(m_cdf);
   m_cdf = new real_t[m_n];
   
   real_t totalPower = 0;
   m_area = 0;
   
   // aggregate emittance CDF
   for(unsigned i = m_n; i--;) {
      Shape *light = (*m_lights)[i];
      const real_t power = 
         light->getMaterial()->getEmitter()->getPower().getAverage();
      
      m_cdf[i]    = power;
      totalPower += power;
      m_area     += light->getSurfaceArea();
   }
   
   // normalize CDF
   ASSERT(totalPower > 0);
   for(unsigned i = m_n; i--;)
      m_cdf[i] /= totalPower;
}

Event EmitterSampler::sample() {
   ASSERT(m_n > 0);
   
   // sample a light source index from the normalized CDF
   const unsigned emitterIndex = Random::sampleNormalizedCDF(m_cdf, m_n);
   ASSERT(emitterIndex < m_n);
   SurfacePoint *pt = new SurfacePoint();
   
   (*m_lights)[emitterIndex]->getRandomPoint(*pt);
   return Event(pt, this, emitterIndex);
}

real_t EmitterSampler::getPd(const Event &event) {
   ASSERT(m_n > 0);
   unsigned emitterIndex = m_n;
   
   if (event.hasMetadata()) {
      emitterIndex = event.getMetadata<unsigned>();
   } else {
      const SurfacePoint *pt = event.getValue<SurfacePoint*>();
      
      // determine which light source the given event lies on
      for(unsigned i = m_n; i--;) {
         if (pt->shape == (*m_lights)[i]) {
            emitterIndex = i;
            break;
         }
      }
   }
   
   ASSERT(emitterIndex < m_n);
   if (emitterIndex >= m_n)
      emitterIndex = m_n - 1;
   
   // sampling with respect to emitted power, but 
   Shape *shape  = (*m_lights)[emitterIndex];
   real_t area   = shape->getSurfaceArea();
   if (area <= EPSILON)
      area = 1;
   
   // TODO: verify correctness
   //real_t pPower = m_cdf[emitterIndex];
   real_t pArea  = (m_area > 0 ? 1.0 / m_area : 1);
   //real_t pConv  = pPower / pArea; // / area
   
   return pArea;//pConv;
}

}

