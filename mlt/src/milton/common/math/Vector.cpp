/**<!-------------------------------------------------------------------->
   @file   Vector.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides basic functionality for a constant-sized Vector
   <!-------------------------------------------------------------------->**/

#include "Vector.h"
#include <Random.h>

namespace milton {

// intentionally almost completely empty (see Vector.inl for implementation)

// Samples a random vector distributed according to a cosine-falloff about 
// the given normal vector (within the upward hemisphere defined by the 
// normal)
template <>
Vector3 Vector3::cosRandom(const Vector3 &normal) {
   const real_t theta = acos(sqrt(Random::sample()));
   const real_t phi   = 2.0 * M_PI * Random::sample();
   
   ASSERT(theta >= -EPSILON && theta <= M_PI / 2 + EPSILON);
   ASSERT(phi   >= -EPSILON && phi   <= M_PI * 2 + EPSILON);
   
   return convertHemisphere(theta, phi, normal);
}

template <unsigned N, typename T>
Vector3 Vector<N, T>::cosRandom(const Vector3 &normal) {
   NYI();
   return Vector3::zero();
}

// force explicit template specialization
template struct Vector<4, real_t>;
template struct Vector<3, real_t>;
template struct Vector<2, real_t>;

}

