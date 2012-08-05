/**<!-------------------------------------------------------------------->
   @file   OmniEmitter.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Point emitter / light which emits light uniformly in every direction, 
   defined at a single point in 3-space (omnidirectional)
   <!-------------------------------------------------------------------->**/

#include "OmniEmitter.h"
#include "Material.h"

namespace milton {

SpectralSampleSet OmniEmitter::getLe(const Vector3 &wo) {
   return m_radiantExitance / (4 * M_PI);
}

}

