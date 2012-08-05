/**<!-------------------------------------------------------------------->
   @class  Vector
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides basic functionality for a constant-sized Vector
   <!-------------------------------------------------------------------->**/

#ifndef VECTOR_H_
#define VECTOR_H_

#include <common/common.h>
#include <iostream>
#include <limits>

#define VECTOR_NO_ELEMENTS    (N)
#define VECTOR_SIZE           (sizeof(T) * VECTOR_NO_ELEMENTS)

namespace milton {

/**
 * @brief
 *    A Vector is templated to store N elements of data of type T.  If 
 * left off, T will default to a "real_t" data type, where a real_t is 
 * assumed to be either a float or a double.
 * 
 * Included in this definition are typedefs for the most commonly used 
 * vectors (Vector4, Vector3, etc), and these can be thought of as shortcuts 
 * to reference their associated Vector templates.  For example, 
 * Vector4 can be used to refer to Vector<4, real_t>.
 * 
 * A Vector stores its data internally as an N-length array of type T.
 */
template <unsigned N = 3, typename T = real_t>
struct MILTON_DLL_EXPORT Vector {
   
   /// Underlying data stored as an N-length array of type T
   T data[VECTOR_NO_ELEMENTS];
   
   
   ///@name Constructors
   //@{-----------------------------------------------------------------
   
   /// Expects N arguments of type T
   inline Vector(const T *srcData);
   
   /// Constructs a zero Vector
   inline Vector();
   
   /// Copy Constructor
   inline Vector(const Vector<N, T> &v);
   
   
   //@}-----------------------------------------------------------------
   ///@name Static convenience constructors to generate common vectors
   //@{-----------------------------------------------------------------
   
   /// Generates a vector full of zeroes
   static inline Vector<N, T> zero() {
      return Vector<N, T>();
   }
   
   /// Generates a vector full of ones
   static inline Vector<N, T> ones() {
      return Vector<N, T>::fill(1);
   }
   
   /// Generates a vector filled with the specified value
   static inline Vector<N, T> fill(const T &val) {
      Vector<N, T> ret = Vector<N, T>::zero();
      
      for(unsigned i = N; i--;)
         ret[i] = val;
      
      return ret;
   }
   
   /// @returns the element-wise minimum vector from the two given vectors
   static inline Vector<N, T> min(const Vector<N, T> &v1, const Vector<N, T> &v2) {
      Vector<N, T> ret;
      
      for(unsigned i = N; i--;)
         ret[i] = MIN(v1[i], v2[i]);

      return ret;
   }
   
   /// @returns the element-wise maximum vector from the two given vectors
   static inline Vector<N, T> max(const Vector<N, T> &v1, const Vector<N, T> &v2) {
      Vector<N, T> ret;
      
      for(unsigned i = N; i--;)
         ret[i] = MAX(v1[i], v2[i]);

      return ret;
   }

   /// Generates a vector filled with the maximum T (infinity)
   static inline Vector<N, T> infinity() {
      return Vector<N, T>::fill(std::numeric_limits<T>::max());
   }
   
   /// Generates a vector filled with the minimum T (negative infinity)
   static inline Vector<N, T> negativeInfinity() {
      // note std::numeric_limits<T>::min() for float and double returns 
      // the smallest positive number representable, not the smallest 
      // valid floating point number that type can hold...
      return Vector<N, T>::fill(-std::numeric_limits<T>::max());
   }
   
   /// theta is latitude angle; phi is longitude angle
   static inline Vector<3, T> fromSpherical(T theta, T phi) {
      return Vector<3, T>(sin(theta) * cos(phi), 
                          cos(theta), 
                          sin(theta) * sin(phi)).getNormalized();
   }
   
   /// Samples a random vector distributed according to a cosine-falloff about 
   /// the given normal vector (within the upward hemisphere defined by the 
   /// normal)
   static Vector<3, real_t> cosRandom(const Vector<3, real_t> &normal);
   
   
   //@}-----------------------------------------------------------------
   ///@name Accessor Operators
   //@{-----------------------------------------------------------------

   /// @returns a const reference to the element at the given index
   inline const T &operator[](const unsigned index) const;
   /// @returns a       reference to the element at the given index
   /// @note changes to the returned element will affect this vector
   inline       T &operator[](const unsigned index);
   
   /// @returns a pointer to the underlying data (N-length array of type T)
   inline const T *operator*() const;
   /// @returns a pointer to the underlying data (N-length array of type T)
   inline       T *operator*();
   
   
   //@}-----------------------------------------------------------------
   ///@name Equality Operators
   //@{-----------------------------------------------------------------
   
   inline       bool       operator==(const Vector<N, T> &v) const;
   inline       bool       operator!=(const Vector<N, T> &v) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name Mutator Operators
   //@{-----------------------------------------------------------------
   
   inline       Vector<N, T> &operator =(const Vector<N, T> &v);
   inline       Vector<N, T> &operator+=(const Vector<N, T> &rhs);
   inline       Vector<N, T> &operator-=(const Vector<N, T> &rhs);
   
   
   //@}-----------------------------------------------------------------
   ///@name Scalar Mutator Operators
   //@{-----------------------------------------------------------------
   
   inline       Vector<N, T> &operator*=(const T &scale);
   inline       Vector<N, T> &operator/=(const T &scale);
   
   
   //@}-----------------------------------------------------------------
   ///@name Arithmetic Operators
   //@{-----------------------------------------------------------------
   
   inline       Vector<N, T>  operator+ (const Vector<N, T> &rhs) const;
   inline       Vector<N, T>  operator- (const Vector<N, T> &rhs) const;
   
   /// @returns element-wise multiplication of two vectors
   inline       Vector<N, T>  operator* (const Vector<N, T> &rhs) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name Scalar Arithmetic Operators
   //@{-----------------------------------------------------------------
   
   inline       Vector<N, T>  operator* (const T &scale) const;
   inline       Vector<N, T>  operator/ (const T &scale) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name More Complex Functionality
   //@{-----------------------------------------------------------------
   
   /// @returns whether or not this Vector is unitized
   inline bool isUnit() const;
   
   /// @returns whether or not this Vector is the zero vector
   inline bool isZero() const;
   
   /// @returns a normalized version of this vector
   inline Vector<N, T> getNormalized() const;   
    
   /// @returns the reciprocal version of this vector (1.0 / this)
   inline Vector<N, T> getReciprocal() const;
   
   /// Normalizes this Vector and returns the old magnitude
   inline T normalize();
   
   /// @returns the magnitude of this vector
   inline T getMagnitude() const;
   
   /// @returns the squared magnitude of this vector
   inline T getMagnitude2() const;
   
   /// @returns the magnitude of the vector connecting this vector to the one 
   ///    passed in
   inline T getDistance(const Vector<N, T> &v) const; 
   
   /// @returns the squared magnitude of the vector connecting this vector to 
   ///    the one passed in
   inline T getDistance2(const Vector<N, T> &v) const;
   
   /// @returns the sum of the components in this Vector
   inline T getSum() const;
   
   /// @returns the average of the components in this Vector
   inline T getAverage() const;
   
   /// @returns the dot (inner) product of two vectors
   inline T dot(const Vector<N, T> &rhs) const;
   
   /// @returns dimension (0,1,...,N) of maximum length
   inline unsigned getMaxDimension() const;
   
   /// @returns dimension (0,1,...,N) of minimum length
   inline unsigned getMinDimension() const;
   
   /// Cleans up vector (0's out entries that are less than epsilon)
   void cleanup();
   
   
   //@}-----------------------------------------------------------------
   ///@name Specialized Vector Functionality
   //@{-----------------------------------------------------------------
   
   /// Convenience Constructor
   inline explicit Vector(const T &v0, const T &v1 = 0, const T &v2 = 0);
   
   /// @returns the cross product of two vectors
   /// @note only have to implement for 3 or 4 dimensions
   ///    (in either case, just use hardcoded cross product for 3 dimensions and 
   ///     if N is 4, set 'w' to 0)
   inline Vector<N, T> cross(const Vector<N, T> &rhs) const;
   
   /**
    * Assumes normal is normalized, incident vector is incoming and
    * resultant vector is exiting
    * 
    * @note only defined for Vector3
    */
   Vector<N, T> reflectVector(const Vector<N, T> &normal) const;
   Vector<N, T> refractVector(const Vector<N, T> &normal, real_t in, 
                              real_t out = 1.0f) const;
   
   /**
    * @returns an orthonormal basis constructed from this vector in out 
    *    parameters U and V
    * 
    * @note only defined for Vector3
    */
   void getOrthonormalBasis(Vector<N, T> &U, Vector<N, T> &V);
   
   
   //@}-----------------------------------------------------------------
};


/// @brief Standard 4-element vector of real_ts
typedef Vector<4> Vector4;

/// @brief Standard 3-element vector of real_ts
typedef Vector<3> Vector3;

/// @brief Standard 2-element vector of real_ts
typedef Vector<2> Vector2;

///@name Extra operators where Vector is on right-hand side
//@{---------------------------------------------------------------------

/// @returns the N-length vector resulting from multiplying a scalar by an 
///    N-length vector
template <unsigned N, typename T>
MILTON_DLL_EXPORT
   inline Vector<N, T> operator* (const real_t &scale, const Vector<N, T> &v);

/// @returns (-1) * rhs, which is a negated version of the original right 
///    hand side vector
template <unsigned N, typename T>
MILTON_DLL_EXPORT
   inline Vector<N, T> operator- (const Vector<N, T> &rhs);

/// Prints a Vector to an output stream
template <unsigned N, typename T>
MILTON_DLL_EXPORT
   inline std::ostream     &operator<<(std::ostream &os, const Vector<N, T> &v);

//@}---------------------------------------------------------------------

/// converts the vector defined by the given spherical coordinates around 
/// the canonical up vector (0, 1, 0) to be centered around the given up 
/// vector N
MILTON_DLL_EXPORT inline Vector3 convertHemisphere(const real_t theta, 
                                                   const real_t phi, 
                                                   const Vector3 &N);

/// converts the vector w centered around the canonical up vector (0, 1, 0) 
/// to be centered around the given up vector N
MILTON_DLL_EXPORT inline Vector3 convertHemisphere(const Vector3 &w, 
                                                   const Vector3 &normal);

}

#if MILTON_ENABLE_SSE
#  include <common/math/simd/Vector3SSE.h>
#else
#  include <common/math/sisd/Vector3.h>
#endif

/* Include inline implementations */
#include "common/math/Vector.inl"

#endif // VECTOR_H_

