/**<!-------------------------------------------------------------------->
   @file   Vector3SSE.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Specialized implementation of Vector3, accelerated to support efficient 
   SIMD vectorization via SSE intrinsics
   
   @note this file is not meant to be #included directly
   @see Vector.h for more details
   <!-------------------------------------------------------------------->**/

#include <Vector.h>
#include <common.h>

namespace milton {

#if MILTON_ENABLE_SSE

// Cleans up vector (0's out entries that are less than epsilon)
void Vector<3, real_t>::cleanup() {
   for(unsigned i = 3; i--;) {
      if (EQ(data[i], create_real(0))) {
         data[i] = 0;
      }
   }
}

/* Assume normal is normalized, incident vector is incoming and
 * resultant vector is exiting */
Vector<3, real_t> Vector<3, real_t>::reflectVector(
   const Vector<3, real_t> &normal) const
{
   return -2 * normal * dot(normal) + *this;
}

Vector<3, real_t> Vector<3, real_t>::refractVector(
   const Vector<3, real_t> &normal, 
   real_t in, real_t out) const
{
   const Vector3 &w = -*this;
   Vector3 norm;
   real_t n;
   
   if (dot(normal) < 0) {  // wi incident on front-side of surface
      ASSERT(out != 0);
      norm = normal;
      n = in / out;
   } else {                // wi incident on back-side  of surface
      ASSERT(in != 0);
      norm = -normal;
      n = out / in;
   }
   
   const real_t d   = w.dot(norm);
   const real_t det = 1 - n * n * (1 - d * d);
   
   if (det < 0)
      return Vector3::zero();
   
   const Vector3 &wo = -n * w + norm * (n * d - sqrt(det));
   ASSERT(wo.isUnit());
   
   return wo;
}

void Vector<3, real_t>::getOrthonormalBasis(Vector<3, real_t> &U, 
                                            Vector<3, real_t> &V) 
{
   normalize();
   const Vector3 &normal = *this;
   Vector3 up = Vector3(0, 1, 0);
   
   if (ABS(up.dot(normal)) > create_real(0.8))
      up = Vector3(1, 0, 0);
   
   U = up.cross(normal).getNormalized();
   V = normal.cross(U).getNormalized();
}

#endif

}

