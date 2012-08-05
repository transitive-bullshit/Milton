/**<!-------------------------------------------------------------------->
   @file   OrientedEmitter.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Area emitter/light which emits light uniformly across a 2D domain, 
   defined at a single point in 3-space, and restricted to the local positive 
   hemisphere (oriented along hemisphere about local surface normal)
   <!-------------------------------------------------------------------->**/

#include "OrientedEmitter.h"
#include "Material.h"

namespace milton {

SpectralSampleSet OrientedEmitter::getLe(const Vector3 &wo) {
   const real_t d = m_pt.normal.dot(wo);
   if (d > 0)
      return m_radiantExitance / M_PI;
   
   return SpectralSampleSet::black();
}

void OrientedEmitter::setWi(const Vector3 &wi) {
   //((BSDF *)m_sampler)->setWi(wi);
   ((BSDF *)m_sampler)->setWi(-((BSDF *)m_sampler)->getSurfacePoint().normalS);
   // TODO: ???
   
   Emitter::setWi(wi);
}

}

