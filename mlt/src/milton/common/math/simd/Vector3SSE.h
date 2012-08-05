/**<!-------------------------------------------------------------------->
   @file   Vector3SSE.h
   @author tRAVIS fIscher (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Specialized implementation of Vector3, accelerated to support efficient 
   SIMD vectorization via SSE intrinsics
   
   @note this file is not meant to be #included directly
   @see Vector.h for more details
   <!-------------------------------------------------------------------->**/

#ifndef VECTOR3_SSE_H_
#define VECTOR3_SSE_H_

#include <common/math/simd/SIMD.h>

#define VECTOR3_NO_ELEMENTS    4
#define VECTOR3_SIZE           (sizeof(real_t) * VECTOR3_NO_ELEMENTS)

namespace milton {

/**
 * @brief
 *    Specialized template implementation for Vector3, accelerated to support 
 * efficient SIMD vectorization via SSE intrinsics
 * 
 * @note implementation uses single-precision floating point arithmetic
 * @see for more information on SSE and SIMD:
 *    ftp://download.intel.com/support/performancetools/c/linux/v9/intref_cls.pdf
 */
template <>
SSE_ALIGN16_PRE struct MILTON_DLL_EXPORT Vector<3, real_t> : public SSEAligned {
   
   SSE_ALIGN16_PRE union {
      /// Underlying data stored as an N-length array of type T
      float   data[VECTOR3_NO_ELEMENTS]; // padded to 16 bytes
      
      /// Also accessible via a native SSE 128-bit register
      m128f_t vec;
      
      struct { real_t x, y, z, w; };
   } SSE_ALIGN16_POST;
   
   
   ///@name Constructors
   //@{-----------------------------------------------------------------
   
   /// Expects N arguments of type float
   inline Vector(const real_t *srcData);
   
   /// Constructs a zero Vector
   inline Vector();
   
   /// Copy Constructor
   inline Vector(const Vector<3, real_t> &v);
   
   /// SSE copy constructor
   inline Vector(const m128f_t &v);
   
   
   //@}-----------------------------------------------------------------
   ///@name Overridden memory operations for alignment guarantees
   //@{-----------------------------------------------------------------
   
   /// @see also SIMD.h
   DECLARE_ALIGNED_MEMORY_OPERATORS
   
   
   //@}-----------------------------------------------------------------
   ///@name Static convenience constructors to generate common matrices
   //@{-----------------------------------------------------------------
   
   /// Generates a vector full of zeroes
   static inline Vector<3, real_t> zero() {
      return Vector<3, real_t>();
   }
   
   /// Generates a vector full of ones
   static inline Vector<3, real_t> ones() {
      return Vector<3, real_t>::fill(1);
   }
   
   /// Generates a vector filled with the specified value
   static inline Vector<3, real_t> fill(const real_t &val) {
      return Vector<3, real_t>(_mm_set_ps1(val));
   }
   
   /// @returns the element-wise minimum vector from the two given vectors
   static inline Vector<3, real_t> min(const Vector<3, real_t> &v1, 
                                       const Vector<3, real_t> &v2)
   {
      return Vector<3, real_t>(_mm_min_ps(v1.vec, v2.vec));
   }
   
   /// @returns the element-wise maximum vector from the two given vectors
   static inline Vector<3, real_t> max(const Vector<3, real_t> &v1, 
                                       const Vector<3, real_t> &v2)
   {
      return Vector<3, real_t>(_mm_max_ps(v1.vec, v2.vec));
   }
   
   /// Generates a vector filled with the maximum real_t (infinity)
   static inline Vector<3, real_t> infinity() {
      return Vector<3, real_t>::fill(std::numeric_limits<real_t>::max());
   }
   
   /// Generates a vector filled with the minimum real_t (negative infinity)
   static inline Vector<3, real_t> negativeInfinity() {
      // note std::numeric_limits<real_t>::min() for float and double returns 
      // the smallest positive number representable, not the smallest 
      // valid floating point number that type can hold...
      return Vector<3, real_t>::fill(-std::numeric_limits<real_t>::max());
   }
   
   /// theta is latitude angle; phi is longitude angle
   static inline Vector<3, real_t> fromSpherical(real_t theta, real_t phi) {
      return Vector<3, real_t>(sin(theta) * cos(phi), 
                               cos(theta), 
                               sin(theta) * sin(phi)).getNormalized();
   }
   
   
   //@}-----------------------------------------------------------------
   ///@name Accessor Operators
   //@{-----------------------------------------------------------------

   /// @returns a const reference to the element at the given index
   inline const real_t &operator[](const unsigned index) const;
   /// @returns a       reference to the element at the given index
   /// @note changes to the returned element will affect this vector
   inline       real_t &operator[](const unsigned index);
   
   /// @returns a pointer to the underlying data (N-length array of type real_t)
   inline const real_t *operator*() const;
   /// @returns a pointer to the underlying data (N-length array of type real_t)
   inline       real_t *operator*();
   
   
   //@}-----------------------------------------------------------------
   ///@name Implicit casting Operators
   //@{-----------------------------------------------------------------
   
   inline operator m128f_t() const {
      return vec;
   }
   
   
   //@}-----------------------------------------------------------------
   ///@name Equality Operators
   //@{-----------------------------------------------------------------
   
   inline       bool       operator==(const Vector<3, real_t> &v) const;
   inline       bool       operator!=(const Vector<3, real_t> &v) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name Mutator Operators
   //@{-----------------------------------------------------------------
   
   inline       Vector<3, real_t> &operator =(const Vector<3, real_t> &v);
   inline       Vector<3, real_t> &operator+=(const Vector<3, real_t> &rhs);
   inline       Vector<3, real_t> &operator-=(const Vector<3, real_t> &rhs);
   
   
   //@}-----------------------------------------------------------------
   ///@name Scalar Mutator Operators
   //@{-----------------------------------------------------------------
   
   inline       Vector<3, real_t> &operator*=(const real_t &scale);
   inline       Vector<3, real_t> &operator/=(const real_t &scale);
   
   
   //@}-----------------------------------------------------------------
   ///@name Arithmetic Operators
   //@{-----------------------------------------------------------------
   
   inline       Vector<3, real_t>  operator+ (const Vector<3, real_t> &rhs) const;
   inline       Vector<3, real_t>  operator- (const Vector<3, real_t> &rhs) const;
   
   /// @returns element-wise multiplication of two vectors
   inline       Vector<3, real_t>  operator* (const Vector<3, real_t> &rhs) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name Scalar Arithmetic Operators
   //@{-----------------------------------------------------------------
   
   inline       Vector<3, real_t>  operator* (const real_t &scale) const;
   inline       Vector<3, real_t>  operator/ (const real_t &scale) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name More Complex Functionality
   //@{-----------------------------------------------------------------
   
   /// @returns whether or not this Vector is unitized
   inline bool isUnit() const;
   
   /// @returns whether or not this Vector is the zero vector
   inline bool isZero() const;
   
   /// @returns a normalized version of this vector
   inline Vector<3, real_t> getNormalized() const;   
   
   /// @returns the reciprocal version of this vector (1.0f / this)
   inline Vector<3, real_t> getReciprocal() const;
   
   /// Normalizes this Vector and returns the old magnitude
   inline real_t normalize();
   
   /// @returns the magnitude of this vector
   inline real_t getMagnitude() const;
   
   /// @returns the squared magnitude of this vector
   inline real_t getMagnitude2() const;
   
   /// @returns the magnitude of the vector connecting this vector to the one 
   ///    passed in
   inline real_t getDistance(const Vector<3, real_t> &v) const; 
   
   /// @returns the squared magnitude of the vector connecting this vector to 
   ///    the one passed in
   inline real_t getDistance2(const Vector<3, real_t> &v) const;
   
   /// @returns the sum of the components in this Vector
   inline real_t getSum() const;
   
   /// @returns the average of the components in this Vector
   inline real_t getAverage() const;
   
   /// @returns the dot (inner) product of two vectors
   inline real_t dot(const Vector<3, real_t> &rhs) const;
   
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
   inline explicit Vector(const real_t &v0, const real_t &v1 = 0, 
                          const real_t &v2 = 0);
   
   /// @returns the cross product of two vectors
   inline Vector<3, real_t> cross(const Vector<3, real_t> &rhs) const;
   
   /**
    * Assumes normal is normalized, incident vector is incoming and
    * resultant vector is exiting
    * 
    * @note only defined for Vector3
    */
   Vector<3, real_t> reflectVector(const Vector<3, real_t> &normal) const;
   Vector<3, real_t> refractVector(const Vector<3, real_t> &normal, 
                                   real_t in, real_t out = 1.0f) const;
   
   /**
    * @returns an orthonormal basis constructed from this vector in out 
    *    parameters U and V
    * 
    * @note only defined for Vector3
    */
   void getOrthonormalBasis(Vector<3, real_t> &U, Vector<3, real_t> &V);
   
   
   //@}-----------------------------------------------------------------
} SSE_ALIGN16_POST;

}

/* Include inline implementations */
#include <common/math/Vector3SSE.inl>

#endif // VECTOR3_SSE_H_

