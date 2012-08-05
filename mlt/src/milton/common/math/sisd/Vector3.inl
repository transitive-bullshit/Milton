/**<!-------------------------------------------------------------------->
   @file   Vector3.inl
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Standard specialized implementation of a 3-element vector
   
   @note this file is not meant to be #included directly
   @see Vector.h for more details
   <!-------------------------------------------------------------------->**/

#ifndef VECTOR3_INL_
#define VECTOR3_INL_

namespace milton {

inline Vector3 convertHemisphere(const Vector3 &w, const Vector3 &normal) {
   Vector3 U, V, N = normal;
   N.getOrthonormalBasis(U, V);
   
   return (U * w[0] + N * w[1] + V * w[2]);
   /*const Vector3 &NL  = Vector3(0, 1, 0); // local normal
   const real_t angle = acos(NL.dot(N));
   
   const Vector3 &u   = NL.cross(N);
   
   if (!u.isUnit())
      return w;
   
   const Vector3 &wo  = getRotMat(Point3(), u, angle) * w;
   ASSERT(wo.dot(N) > -EPSILON);
   
   return wo;*/
}

}

#endif // VECTOR3_INL_

