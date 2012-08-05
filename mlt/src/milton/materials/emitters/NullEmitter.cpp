/**<!-------------------------------------------------------------------->
   @file   NullEmitter.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Null emitter which doesn't emit any light, defined at a single point 
   in 3-space
   <!-------------------------------------------------------------------->**/

#include "NullEmitter.h"

namespace milton {

void NullEmitter::init() {
   m_power           = SpectralSampleSet::black();
   m_radiantExitance = SpectralSampleSet::black();
}

SpectralSampleSet NullEmitter::getLe(const Vector3 &) {
   return SpectralSampleSet::black();
}

void NullEmitter::preview(Shape *) {
   // intentionally empty
}

}

