/**<!-------------------------------------------------------------------->
   @file   UniformOnSphereSampler.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a uniform distribution on the surface of an N-dimensional 
   unit sphere (with parameter N > 0 and radius = 1)
   <!-------------------------------------------------------------------->**/

#include "UniformOnSphereSampler.h"
#include <Point.h>

namespace milton {

void UniformOnSphereSampler::init() {
   Sampler::init();
   
   ASSERT(m_dimension > 0);
}

Event UniformOnSphereSampler::sample() {
   const Random::UniformOnSphereDist::result_type &s = m_sampler();
   ASSERT(s.size() == m_dimension);
   
   ASSERT(m_dimension == 3);
   // TODO: make this not tied to 3 dimensions...
   
   return Event(Point3(s[0], s[1], s[2]), this);
}

real_t UniformOnSphereSampler::getPd(const Event &event) {
   return 1;
   
   // Note by definition, uniform on sphere is uniform w/ pdf 1
   /*const unsigned int N = m_dimension;
   const real_t surfaceArea = 2.0 * pow(M_PI, N / 2.0) / _factorial(N / 2);
   
   // above formula is only valid for even dimensions because _factorial 
   // assumes it has integer input, and if N is odd, N / 2 will be 
   // non-integer, so we'd have to use the gamma function
   ASSERT(!(N & 1));
   
   return 1.0 / surfaceArea;*/
}

unsigned UniformOnSphereSampler::_factorial(unsigned n) {
   const unsigned limit = n + (n > 2);
   unsigned fact  = 1;
   
   for(unsigned i = 2; i < limit; ++i)
      fact *= i;
   
   return fact;
}

}

