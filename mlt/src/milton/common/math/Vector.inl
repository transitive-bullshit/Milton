/**<!-------------------------------------------------------------------->
   @file   Vector.inl
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides basic functionality for a constant-sized Vector
   <!-------------------------------------------------------------------->**/

#ifndef VECTOR_INL_
#define VECTOR_INL_

namespace milton {


// Constructors
// ------------

// Expects N arguments of type T
// (srcData is a pointer to an N-length array of type T)
template <unsigned N, typename T>
inline Vector<N, T>::Vector(const T *srcData) {
   memcpy(data, srcData, VECTOR_SIZE);
}

// Constructs an zero Vector
template <unsigned N, typename T>
inline Vector<N, T>::Vector() {
   memset(data, 0, VECTOR_SIZE);
}

// Copy Constructor; copies the underlying data from v to this vector
template <unsigned N, typename T>
inline Vector<N, T>::Vector(const Vector<N, T> &v) {
   memcpy(data, v.data, VECTOR_SIZE);
}


// Accessor Operators
// ------------------

// @returns a const reference to the element at the given index
template <unsigned N, typename T>
inline const T &Vector<N, T>::operator[](const unsigned index) const {
   ASSERT(index < N);
   
   return data[index];
}

// @returns a reference to the element at the given index
// @note changes to the returned element will affect this vector
template <unsigned N, typename T>
inline       T &Vector<N, T>::operator[](const unsigned index) {
   ASSERT(index < N);
   
   return data[index];
}

// @returns a pointer to the underlying data (N-length array of type T)
template <unsigned N, typename T>
inline const T *Vector<N, T>::operator* () const {
   return (T*)this;
}

// @returns a pointer to the underlying data (N-length array of type T)
template <unsigned N, typename T>
inline       T *Vector<N, T>::operator* () {
   return (T*)this;
}


// Equality Operators
// ------------------
template <unsigned N, typename T>
inline       bool       Vector<N, T>::operator==(const Vector<N, T> &v) const
{
   for(unsigned i = N; i--;)
      if (NEQ(data[i], v.data[i]))
         return false;
   
   return true;
}

template <unsigned N, typename T>
inline       bool       Vector<N, T>::operator!=(const Vector<N, T> &v) const 
{
   return !((*this) == v);
}

// Mutator Operators
// -----------------

template <unsigned N, typename T>
inline       Vector<N, T> &Vector<N, T>::operator =(const Vector<N, T> &v)
{
   memcpy(data, v.data, VECTOR_SIZE);
   
   return *this;
}

template <unsigned N, typename T>
inline       Vector<N, T> &Vector<N, T>::operator+=(const Vector<N, T> &rhs)
{
   for(unsigned i = N; i--;)
      data[i] += rhs.data[i];
   
   return *this;
}

template <unsigned N, typename T>
inline       Vector<N, T> &Vector<N, T>::operator-=(const Vector<N, T> &rhs)
{
   return (*this += -rhs);
}


// Scalar Mutator Operators
// ------------------------
template <unsigned N, typename T>
inline       Vector<N, T> &Vector<N, T>::operator*=(const T &scale) {
   for(unsigned i = N; i--;)
      data[i] *= scale;
   
   return *this;
}

template <unsigned N, typename T>
inline       Vector<N, T> &Vector<N, T>::operator/=(const T &scale) {
   if (scale != 0) {
      for(unsigned i = N; i--;)
         data[i] /= scale;
   } else {
      std::cerr << "Error: Attempting to divide Vector by zero" << std::endl << std::endl;
      ASSERT(0);
   }
   
   return *this;
}


// Arithmetic Operators
// --------------------

template <unsigned N, typename T>
inline       Vector<N, T>  Vector<N, T>::operator+ (const Vector<N, T> &rhs) const
{
   T d[VECTOR_NO_ELEMENTS];
   
   for(unsigned i = N; i--;)
      d[i] = data[i] + rhs.data[i];
   
   return Vector<N, T>(d);
}

template <unsigned N, typename T>
inline       Vector<N, T>  Vector<N, T>::operator- (const Vector<N, T> &rhs) const
{
   T d[VECTOR_NO_ELEMENTS];
   
   for(unsigned i = N; i--;)
      d[i] = data[i] - rhs.data[i];
   
   return Vector<N, T>(d);
}

template <unsigned N, typename T>
inline       Vector<N, T>  Vector<N, T>::operator* (const Vector<N, T> &rhs) const
{
   T d[VECTOR_NO_ELEMENTS];
   
   for(unsigned i = N; i--;)
      d[i] = data[i] * rhs.data[i];
   
   return Vector<N, T>(d);
}

// Scalar Arithmetic Operators
template <unsigned N, typename T>
inline       Vector<N, T>  Vector<N, T>::operator* (const T &scale) const {
   T d[VECTOR_NO_ELEMENTS];
   
   for(unsigned i = N; i--;)
      d[i] = data[i] * scale;
   
   return Vector<N, T>(d);
}

template <unsigned N, typename T>
inline       Vector<N, T>  Vector<N, T>::operator/ (const T &scale) const {
   if (scale != 0) {
      return (*this * (create_real(1.0) / scale));
   } else {
      std::cerr << "Error: Attempting to divide Vector by zero" << std::endl << std::endl;
      ASSERT(0);
      
      return Vector<N, T>();
   }
}


// More Complex Functionality
// --------------------------

// @returns whether or not this Vector is unitized
template <unsigned N, typename T>
inline bool Vector<N, T>::isUnit() const {
   return (EQ(getMagnitude2(), static_cast<T>(1)));
}

// @returns whether or not this Vector is the zero vector
template <unsigned N, typename T>
inline bool Vector<N, T>::isZero() const {
   for(unsigned i = N; i--;) {
      if (data[i] != 0)
         return false;
   }
   
   return true;
}

// @returns a normalized version of this vector
template <unsigned N, typename T>
inline Vector<N, T> Vector<N, T>::getNormalized() const {
   const T &magnitude = getMagnitude();
   
   if (magnitude == 0) {
      //std::cerr << "Error: Attempting to normalize vector with magnitude zero" << std::endl << std::endl;
      //ASSERT(0);
      
      // TODO: how to handle this; apparently lots of meshes contain zero-ish data
      // which gets truncated to zero and we get here..
      
      return Vector<N, T>();
   } else {
      const real_t &inv  = create_real(1.0) / magnitude;
      T d[VECTOR_NO_ELEMENTS]; 
      
      for(unsigned i = N; i--;)
         d[i] = data[i] * inv;
      
      return Vector<N, T>(d);
   }
}

// @returns the reciprocal version of this vector (1.0 / this)
template <unsigned N, typename T>
inline Vector<N, T> Vector<N, T>::getReciprocal() const {
   T d[VECTOR_NO_ELEMENTS];
   
   for(unsigned i = N; i--;)
      d[i] = create_real(1.0) / data[i];
   
   return Vector<N, T>(d);
}

// Normalizes this Vector and returns the old magnitude
template <unsigned N, typename T>
inline T Vector<N, T>::normalize() {
   const T &magnitude = getMagnitude();
   
   if (magnitude != 0) {
      const real_t &inv  = 1.0 / magnitude;
      
      for(unsigned i = N; i--;)
         data[i] *= inv;
   } else {
      //std::cerr << "Error: Attempting to normalize vector with magnitude zero" << std::endl << std::endl;
      //ASSERT(0);
      // TODO: how to handle this; apparently lots of meshes contain zero-ish data
      // which gets truncated to zero and we get here..
   }
   
   return magnitude;
}

// @returns the magnitude of this vector
template <unsigned N, typename T>
inline T Vector<N, T>::getMagnitude() const {
   return sqrt(getMagnitude2());
}

// @returns the squared magnitude of this vector
template <unsigned N, typename T>
inline T Vector<N, T>::getMagnitude2() const {
   T magnitude2 = 0;
   
   for(unsigned i = N; i--;)
      magnitude2 += data[i] * data[i];
   
   return magnitude2;
}

// @returns the magnitude of the vector connecting this vector to the one 
//    passed in
template <unsigned N, typename T>
inline T Vector<N, T>::getDistance(const Vector<N, T> &v) const {
   return sqrt(getDistance2(v));
}

// @returns the squared magnitude of the vector connecting this vector to 
//    the one passed in
template <unsigned N, typename T>
inline T Vector<N, T>::getDistance2(const Vector<N, T> &v) const {
   return (v - *this).getMagnitude2();
}

// @returns the sum of the components in this Vector
template <unsigned N, typename T>
inline T Vector<N, T>::getSum() const {
   T sum = 0;
   
   for(unsigned i = N; i--;)
      sum += data[i];
   
   return sum;
}

// @returns the average of the components in this Vector
template <unsigned N, typename T>
inline T Vector<N, T>::getAverage() const {
   return (getSum() / N);
}

// @returns the dot (inner) product of two vectors
template <unsigned N, typename T>
inline T Vector<N, T>::dot(const Vector<N, T> &rhs) const {
   T result = 0;
   
   for(unsigned i = N; i--;)
      result += data[i] * rhs[i];
   
   return result;
}


// Specialized Vector Functionality
// --------------------------------

// Convenience Constructor
template <unsigned N, typename T>
inline Vector<N, T>::Vector(const T &v0, const T &v1, const T &v2) {
   if (N > 0)
      data[0] = v0;
   if (N > 1)
      data[1] = v1;
   if (N > 2)
      data[2] = v2;
}

// @returns the cross product of two vectors
// @note only have to implement for 3 or 4 dimensions
//    (in either case, just use hardcoded cross product for 3 dimensions and 
//     if N is 4, set 'w' to 0)
template <unsigned N, typename T>
inline Vector<N, T> Vector<N, T>::cross(const Vector<N, T> &v) const
{
   const Vector<N, T> &u = *this;
   
   if (N == 3 || N == 4) {
      return Vector<N, T>(u[1] * v[2] - u[2] * v[1], 
                          u[2] * v[0] - u[0] * v[2], 
                          u[0] * v[1] - u[1] * v[0]);
   }
   
   ASSERT(0 && "cross products are only implemented for N=3 and N=4\n");
   return Vector<N, T>();
}


// Extra operators where Vector is on right-hand side
// --------------------------------------------------

// @returns the N-length vector resulting from multiplying a scalar by an 
//    N-length vector
template <unsigned N, typename T>
inline       Vector<N, T>  operator* (const real_t &scale, 
                                      const Vector<N, T> &rhs)
{
   return rhs * scale;
}

// @returns (-1) * rhs, which is a negated version of the original right 
//    hand side vector
template <unsigned N, typename T>
inline       Vector<N, T> operator- (const Vector<N, T> &rhs) {
   return rhs * (-1);
}

// Prints a Vector to an output stream
template <unsigned N, typename T>
inline       std::ostream      &operator<<(std::ostream &os, 
                                           const Vector<N, T> &v)
{
   os << "[ ";
   
   for(unsigned i = 0; i < N; ++i)
      os << v.data[i] << (i < N - 1 ? ", " : "");

   os << " ]";
   return os;
}

// @returns dimension (0,1,...,N) of maximum length
template <unsigned N, typename T>
inline unsigned Vector<N, T>::getMaxDimension() const {
   unsigned max = 0;
   T maxVal = 0;
   
   for(unsigned i = N; i--;) {
      const T &val = ABS(data[i]);
      
      if (val > maxVal) {
         max = i;
         maxVal = val;
      }
   }
   
   return max;
}

// @returns dimension (0,1,...,N) of minimum length
template <unsigned N, typename T>
inline unsigned Vector<N, T>::getMinDimension() const {
    unsigned min = N - 1;
   
   for(unsigned i = N - 1; i--;)
      if (data[i] < data[min])
         min = i;
   
   return min;
}

inline Vector3 convertHemisphere(const real_t theta, const real_t phi, 
                                 const Vector3 &N)
{
   ASSERT(theta >= -EPSILON && theta <= M_PI / 2 + EPSILON);
   ASSERT(phi   >= -EPSILON && phi   <= M_PI * 2 + EPSILON);
   
   const Vector3 &w = Vector3::fromSpherical(theta, phi);
   
   return convertHemisphere(w, N);
}


#if (!defined(MILTON_ARCH_WINDOWS) || defined(MILTON_EXPORTS))

// Cleans up vector (0's out entries that are less than epsilon)
template <unsigned N, typename T>
void Vector<N, T>::cleanup() {
   for(unsigned i = N; i--;) {
      if (EQ(data[i], static_cast<T>(0))) {
         data[i] = 0;
      }
   }
}

/* Assume normal is normalized, incident vector is incoming and
 * resultant vector is exiting */
template <unsigned N, typename T>
Vector<N, T> Vector<N, T>::reflectVector(const Vector<N, T> &normal) const {
   return -2 * normal * dot(normal) + *this;
   //const Vector<N, T> &n = (dot(normal) < 0 ? normal : -normal);
   
   //return -2 * n * dot(n) + *this;
}

template <unsigned N, typename T>
Vector<N, T> Vector<N, T>::refractVector(const Vector<N, T> &normal, 
                                         real_t in, real_t out) const
{
   const Vector<N, T> &w = -*this;
   Vector<N, T> norm;
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
      return Vector<N, T>::zero();
   
   const Vector<N, T> &wo = -n * w + norm * (n * d - sqrt(det));
   ASSERT(wo.isUnit());
   
   return wo;
}

#if 0
template <unsigned N, typename T>
Vector<N, T> Vector<N, T>::refractVector(const Vector<N, T> &normal, 
                                         real_t in, real_t out) const
{
   const Vector3 &dir = -*this;
   Vector3 norm;
   real_t n;
   
   if (dot(normal) < 0) {  // wi incident on front-side of surface
      ASSERT(in != 0);
      norm = normal;
      n = out / in;
   } else {                // wi incident on back-side  of surface
      ASSERT(out != 0);
      norm = -normal;
      n = in / out;
   }
   
   const real_t d = dir.dot(norm);
   const real_t det = 1 - n * n * (1 - d * d);
   
   if (det < 0)
      return Vector<3, T>::zero();
   
   return -n * (dir - d * norm) - norm * sqrt(det); 
}
#endif

template <unsigned N, typename T>
void Vector<N, T>::getOrthonormalBasis(Vector<N, T> &U, Vector<N, T> &V) 
{
   normalize();
   const Vector<N, T> &normal = *this;
   Vector<N, T> up = Vector<N, T>(0, 1, 0);
   
   if (ABS(up.dot(normal)) > create_real(0.8))
      up = Vector<N, T>(1, 0, 0);
   
   U = up.cross(normal).getNormalized();
   V = normal.cross(U).getNormalized();
}

#endif

}

#endif // VECTOR_INL_

