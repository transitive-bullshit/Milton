/**<!-------------------------------------------------------------------->
   @file   AbsorbentBSDF.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Ideal, cool, black-body absorbent material (absorbs all incoming light)
   <!-------------------------------------------------------------------->**/

#include "AbsorbentBSDF.h"

namespace milton {

Event AbsorbentBSDF::sample (const Event &) {
   return Event(Vector3(), this);
}

real_t AbsorbentBSDF::getPd(const Event &event) {
   return 0;
}

SpectralSampleSet AbsorbentBSDF::evaluate(const Vector3 &wi, const Vector3 &wo) {
   return SpectralSampleSet::black();
}

}

