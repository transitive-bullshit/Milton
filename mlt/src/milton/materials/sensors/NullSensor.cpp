/**<!-------------------------------------------------------------------->
   @file   NullSensor.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Null sensor which doesn't emit any importance
   <!-------------------------------------------------------------------->**/

#include "NullSensor.h"
#include "Material.h"

namespace milton {

void NullSensor::init() {
   m_camera = NULL;
}

bool NullSensor::isSensor() {
   return false;
}

SpectralSampleSet NullSensor::getWe(const Vector3 &wo) {
   return SpectralSampleSet::black();
}

SpectralSampleSet NullSensor::getImportance() {
   return SpectralSampleSet::black();
}

// TODO: modify Camera interface / implementation to accomodate stochastic 
// sampling w/ deterministic Pinhole camera as a special-case
Event NullSensor::sample(const Event &) {
   return Event(this);
}

real_t NullSensor::getPd(const Event &event) {
   return 0;
}

SpectralSampleSet NullSensor::evaluate(const Vector3 &/* wi unused */, 
                             const Vector3 &/* wo unused */)
{
   return SpectralSampleSet::black();
}

}

