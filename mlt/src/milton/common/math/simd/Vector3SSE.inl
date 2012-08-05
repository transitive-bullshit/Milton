/**<!-------------------------------------------------------------------->
   @file   Vector3SSE.inl
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Specialized implementation of Vector3, accelerated to support efficient 
   SIMD vectorization via SSE intrinsics
   
   @note this file is not meant to be #included directly
   @see Vector.h for more details
   <!-------------------------------------------------------------------->**/

#ifndef VECTOR3_SSE_INL_
#define VECTOR3_SSE_INL_

namespace milton {


// Constructors
// ------------

// Expects N arguments of type T
// (srcData is a pointer to an N-length array of type T)
inline Vector<3, real_t>::Vector(const real_t *srcData)
   : vec(_mm_setr_ps(srcData[0], srcData[1], srcData[2], 0))
{ }

// Constructs an zero Vector
inline Vector<3, real_t>::Vector() 
   : vec(_mm_setzero_ps())
{ }

// Copy Constructor; copies the underlying data from v to this vector
inline Vector<3, real_t>::Vector(const Vector<3, real_t> &v) 
   : vec(v.vec)
{ }

// SSE copy constructor
inline Vector<3, real_t>::Vector(const m128f_t &v)
   : vec(v)
{ }


// Accessor Operators
// ------------------

// @returns a const reference to the element at the given index
inline const real_t &Vector<3, real_t>::operator[](const unsigned index) const {
   ASSERT(index < 3);
   
   return data[index];
}

// @returns a reference to the element at the given index
// @note changes to the returned element will affect this vector
inline       real_t &Vector<3, real_t>::operator[](const unsigned index) {
   ASSERT(index < 3);
   
   return data[index];
}

// @returns a pointer to the underlying data (N-length array of type T)
inline const real_t *Vector<3, real_t>::operator* () const {
   return data;
}

// @returns a pointer to the underlying data (N-length array of type T)
inline       real_t *Vector<3, real_t>::operator* () {
   return data;
}

 
// Equality Operators
// ------------------

inline       bool       Vector<3, real_t>::operator==(
   const Vector<3, real_t> &v) const
{
   return SSE_EQb(vec, v.vec);
}


inline       bool       Vector<3, real_t>::operator!=(
   const Vector<3, real_t> &v) const 
{
   return !(*this == v);
}

// Mutator Operators
// -----------------

inline       Vector<3, real_t> &Vector<3, real_t>::operator =(
   const Vector<3, real_t> &v)
{
   vec = v.vec;
   
   return *this;
}

inline       Vector<3, real_t> &Vector<3, real_t>::operator+=(
   const Vector<3, real_t> &rhs)
{
   vec = _mm_add_ps(vec, rhs.vec);
   
   return *this;
}

inline       Vector<3, real_t> &Vector<3, real_t>::operator-=(
   const Vector<3, real_t> &rhs)
{
   vec = _mm_sub_ps(vec, rhs.vec);
   
   return *this;
}


// Scalar Mutator Operators
// ------------------------

inline       Vector<3, real_t> &Vector<3, real_t>::operator*=(
   const real_t &scale)
{
   vec = _mm_mul_ps(vec, _mm_set_ps1(scale));
   
   return *this;
}

inline       Vector<3, real_t> &Vector<3, real_t>::operator/=(
   const real_t &scale)
{
   if (scale != 0) {
      vec = _mm_div_ps(vec, _mm_set_ps1(scale));
   } else {
      std::cerr << "Error: Attempting to divide Vector by zero" << std::endl << std::endl;
      ASSERT(0);
   }
   
   return *this;
}


// Arithmetic Operators
// --------------------

inline       Vector<3, real_t>  Vector<3, real_t>::operator+ (
   const Vector<3, real_t> &rhs) const
{
   return Vector<3, real_t>(_mm_add_ps(vec, rhs.vec));
}

inline       Vector<3, real_t>  Vector<3, real_t>::operator- (
   const Vector<3, real_t> &rhs) const
{
   return Vector<3, real_t>(_mm_sub_ps(vec, rhs.vec));
}

inline       Vector<3, real_t>  Vector<3, real_t>::operator* (
   const Vector<3, real_t> &rhs) const
{
   return Vector<3, real_t>(_mm_mul_ps(vec, rhs.vec));
}

// Scalar Arithmetic Operators
inline       Vector<3, real_t>  Vector<3, real_t>::operator* (
   const real_t &scale) const 
{
   return Vector<3, real_t>(_mm_mul_ps(vec, _mm_set_ps1(scale)));
}

inline       Vector<3, real_t>  Vector<3, real_t>::operator/ (
   const real_t &scale) const
{
   if (scale != 0) {
      return Vector<3, real_t>(_mm_div_ps(vec, _mm_set_ps1(scale)));
   } else {
      std::cerr << "Error: Attempting to divide Vector by zero" << std::endl << std::endl;
      ASSERT(0);
      
      return Vector<3, real_t>();
   }
}


// More Complex Functionality
// --------------------------

// @returns whether or not this Vector is unitized
inline bool Vector<3, real_t>::isUnit() const {
   return (EQ(getMagnitude2(), create_real(1)));
}

// @returns whether or not this Vector is the zero vector
inline bool Vector<3, real_t>::isZero() const {
   return (_mm_extract_f32(_mm_cmpeq_ps(vec, _mm_setzero_ps())) != 0);
}

// @returns a normalized version of this vector
inline Vector<3, real_t> Vector<3, real_t>::getNormalized() const {
   const real_t &magnitude = getMagnitude();
   
   if (magnitude == 0) {
      //std::cerr << "Error: Attempting to normalize vector with magnitude zero" 
      //<< std::endl << std::endl;
      //ASSERT(0);
      
      // TODO: how to handle this; apparently lots of meshes contain 
      // zero-ish data which gets truncated to zero and we get here..
      
      return Vector<3, real_t>();
   } else {
      return Vector<3, real_t>(_mm_div_ps(vec, _mm_set_ps1(magnitude)));
   }
}

// @returns the reciprocal version of this vector (1.0f / this)
inline Vector<3, real_t> Vector<3, real_t>::getReciprocal() const {
   return Vector<3, real_t>(_mm_rcp_ps(vec));
}

// Normalizes this Vector and returns the old magnitude
inline real_t Vector<3, real_t>::normalize() {
   const real_t &magnitude = getMagnitude();
   
   if (magnitude != 0) {
      vec = _mm_div_ps(vec, _mm_set_ps1(magnitude));
      
#ifdef DEBUG
      const Vector3 &v = *this;
      const real_t s = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
      ASSERT(EQ(sqrt(s), create_real(1)));
#endif
   } else {
      //std::cerr << "Error: Attempting to normalize vector with magnitude zero" 
      //<< std::endl << std::endl;
      //ASSERT(0);
      
      // TODO: how to handle this; apparently lots of meshes contain 
      // zero-ish data which gets truncated to zero and we get here..
   }
   
   return magnitude;
}

// available in SSE4 as a builtin
// returns the dot (inner) product between the two vectors given
static inline real_t s_dot(const m128f_t &v1, const m128f_t &v2) {
   const SimpleSSEVector v(_mm_mul_ps(v1, v2));
   
   return (v.x + v.y + v.z);
   /*const m128f_t &l = _mm_movelh_ps(v, v);
   const m128f_t &h = _mm_movehl_ps(v, v);
   
   return _mm_add_ps(l, h);*/
}

// @returns the magnitude of this vector
inline real_t Vector<3, real_t>::getMagnitude() const {
   const real_t d  = sqrt(getMagnitude2());
   
#if 0 // debugging; alternate way of computing magnitude
   const real_t d1 = _mm_extract_f32(_mm_sqrt_ss(_mm_dot_ps(vec, vec)));
   ASSERT(EQ(d, d1));
#endif
   
   return d;
}

// @returns the squared magnitude of this vector
inline real_t Vector<3, real_t>::getMagnitude2() const {
   // squared magnitude is same as dot product of a vector with itself
   return dot(*this);
}

// @returns the magnitude of the vector connecting this vector to the one 
//    passed in
inline real_t Vector<3, real_t>::getDistance(const Vector<3, real_t> &v) const
{
   return sqrt(getDistance2(v));
}

// @returns the squared magnitude of the vector connecting this vector to 
//    the one passed in
inline real_t Vector<3, real_t>::getDistance2(const Vector<3, real_t> &v) const
{
   const m128f_t &v1 = _mm_sub_ps(v, vec);
   
   return s_dot(v1, v1);
}

// @returns the sum of the components in this Vector
inline real_t Vector<3, real_t>::getSum() const {
   return data[0] + data[1] + data[2];
}

// @returns the average of the components in this Vector
inline real_t Vector<3, real_t>::getAverage() const {
   return (getSum() / 3);
}

// @returns the dot (inner) product of two vectors
inline real_t Vector<3, real_t>::dot(const Vector<3, real_t> &rhs) const {
   return s_dot(vec, rhs.vec);
}


// Specialized Vector Functionality
// --------------------------------

// Convenience Constructor
inline Vector<3, real_t>::Vector(const real_t &v0, const real_t &v1, 
                                 const real_t &v2) 
   : vec(_mm_setr_ps(v0, v1, v2, 0))
{ }

// @returns the cross product of two vectors
inline Vector<3, real_t> Vector<3, real_t>::cross(
   const Vector<3, real_t> &v) const
{
   const Vector<3, real_t> &u = *this;
   
   return Vector<3, real_t>(u[1] * v[2] - u[2] * v[1], 
                            u[2] * v[0] - u[0] * v[2], 
                            u[0] * v[1] - u[1] * v[0]);
}

/*void crossp_sse(float v1[4], float v2[4], float out[4]) {
   __m128 vector1, vector2, vector3, vector4, vector5;
   
   vector1 = _mm_load_ps(v1);
   vector2 = _mm_load_ps(v2);
   
   vector3 = _mm_shuffle_ps(vector2, vector1, _MM_SHUFFLE(3, 0, 2, 2));
   vector4 = _mm_shuffle_ps(vector1, vector2, _MM_SHUFFLE(3, 1, 0, 1));
   
   vector5 = _mm_mul_ps(vector3, vector4);
   
   vector3 = _mm_shuffle_ps(vector1, vector2, _MM_SHUFFLE(3, 0, 2, 2));
   vector4 = _mm_shuffle_ps(vector2, vector1, _MM_SHUFFLE(3, 1, 0, 1));
   
   vector3 = _mm_mul_ps(vector3, vector4);
   vector3 = _mm_sub_ps(vector5, vector3);
   
   _mm_store_ps(out, vector3);
   
   out[1] *= -1;
}*/

// Extra operators where Vector is on right-hand side
// --------------------------------------------------

// @returns the N-length vector resulting from multiplying a scalar by an 
//    N-length vector
inline       Vector<3, real_t>  operator* (const real_t &scale, 
                                      const Vector<3, real_t> &rhs)
{
   return rhs * scale;
}

// @returns (-1) * rhs, which is a negated version of the original right 
//    hand side vector
inline       Vector<3, real_t> operator- (const Vector<3, real_t> &rhs) {
   return Vector<3, real_t>(_mm_sub_ps(_mm_setzero_ps(), rhs.vec));
}

// Prints a Vector to an output stream
inline       std::ostream      &operator<<(std::ostream &os, 
                                           const Vector<3, real_t> &v)
{
   os << "[ ";
   
   for(unsigned i = 0; i < 3; ++i)
      os << v.data[i] << (i < 3 - 1 ? ", " : "");

   os << " ]";
   return os;
}

// @returns dimension (0,1,...,N) of maximum length
inline unsigned Vector<3, real_t>::getMaxDimension() const {
   unsigned max = 0;
   real_t maxVal = 0;
   
   for(unsigned i = 3 - 1; i--;) {
      const real_t &val = ABS(data[i]);
      
      if (val > maxVal) {
         max = i;
         maxVal = val;
      }
   }
   
   return max;
}

// @returns dimension (0,1,...,N) of minimum length
inline unsigned Vector<3, real_t>::getMinDimension() const {
    unsigned min = 3 - 1;
   
   for(unsigned i = 3 - 1; i--;)
      if (data[i] < data[min])
         min = i;
   
   return min;
}

inline Vector3 convertHemisphere(const Vector3 &w, const Vector3 &normal) {
   Vector3 U, V, N = normal;
   N.getOrthonormalBasis(U, V);
   
   return (U * w[0] + N * w[1] + V * w[2]);
}

}

#endif // VECTOR3_SSE_INL_

