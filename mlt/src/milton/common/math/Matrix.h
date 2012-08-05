/**<!-------------------------------------------------------------------->
   @class  Matrix
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides basic functionality for a templated, arbitrarily-sized matrix
   
   A Matrix is templated to store M rows and N columns of data of 
   type T.  If left off, T will default to a "real_t" data type. Also, if N 
   is not specified, the template will default to a square MxM matrix.
   
   Included in this definition are typedefs for the most commonly used 
   matrices (4x4, 3x3, etc), and these can be thought of as shortcuts to 
   reference their associated Matrix templates.  For example, 
   Matrix4x4 can be used to refer to Matrix<4, 4, real_t>.
   
   A Matrix stores each of its M rows internally as an 
   N-length Vector called rows which has been defined as:
   
      Vector<N, T> rows[M];
   <!-------------------------------------------------------------------->**/

#ifndef MATRIX_H_
#define MATRIX_H_

#include <common/math/Point.h>
#include <ostream>

#define MATRIX_NO_ELEMENTS    (M * N)
#define MATRIX_SIZE           (sizeof(T) * MATRIX_NO_ELEMENTS)

namespace milton {

/**
 * @brief
 *    A Matrix is templated to store M rows and N columns of data of 
 * type T.  If left off, T will default to a "real_t" data type. Also, if N 
 * is not specified, the template will default to a square MxM matrix.
 * 
 * Included in this definition are typedefs for the most commonly used 
 * matrices (4x4, 3x3, etc), and these can be thought of as shortcuts to 
 * reference their associated Matrix templates.  For example, 
 * Matrix4x4 can be used to refer to Matrix<4, 4, real_t>.
 * 
 * A Matrix stores each of its M rows internally as an 
 * N-length Vector.
 */
template <unsigned M, unsigned N = M, typename T = real_t>
struct MILTON_DLL_EXPORT Matrix {
   
   /// Underlying data, where each row is stored in a separate Vector
   Vector<N, T> rows[M];
   
   
   ///@name Constructors
   //@{-----------------------------------------------------------------
   
   /// Expects arguments in row-major order
   inline Matrix(const T *srcData);
   
   /// Constructs a zero Matrix
   inline Matrix();
   
   /// Copy Constructor
   inline Matrix(const Matrix<M, N, T> &m);
   
   
   //@}-----------------------------------------------------------------
   ///@name Static convenience constructors to generate common matrices
   //@{-----------------------------------------------------------------
   
   /// Generates a matrix full of zeroes
   static inline Matrix zero() {
      return Matrix<M, N, T>();
   }
   
   /// Generates an identity matrix
   static Matrix identity() {
      return Matrix<M, N, T>::diagonal(1);
   }
   
   /// Generates a diagonal matrix with the value specified
   static Matrix diagonal(T diagonalValue) {
      T d[MATRIX_NO_ELEMENTS];
      
      memset(d, 0, MATRIX_SIZE);
      
      for(unsigned i = M; i--;)
         d[i * N + i] = diagonalValue;
      
      return Matrix<M, N, T>(d);
   }
   
   
   //@}-----------------------------------------------------------------
   ///@name Accessor Operators
   //@{-----------------------------------------------------------------
   
   /// @returns a const reference to the row at the given index
   inline const Vector<N, T>   &operator[](const unsigned rowIndex) const;
   /// @returns a       reference to the row at the given index
   /// @note changes to the returned vector will affect this matrix
   inline       Vector<N, T>   &operator[](const unsigned rowIndex);
   
   /// @returns a const pointer to the raw, underlying data
   ///    (row-major, M*N-length array of type T)
   inline const T *operator*() const;
   /// @returns a pointer to the raw, underlying data
   ///    (row-major, M*N-length array of type T)
   inline       T *operator*();
   
   
   //@}-----------------------------------------------------------------
   ///@name Equality Operators
   //@{-----------------------------------------------------------------
   
   inline       bool       operator==(const Matrix<M, N, T> &m) const;
   inline       bool       operator!=(const Matrix<M, N, T> &m) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name Mutator Operators
   //@{-----------------------------------------------------------------
   
   inline       Matrix<M, N, T> &operator =(const Matrix<M, N, T> &m);
   inline       Matrix<M, N, T> &operator+=(const Matrix<M, N, T> &rhs);
   inline       Matrix<M, N, T> &operator-=(const Matrix<M, N, T> &rhs);
   inline       Matrix<M, N, T> &operator*=(const Matrix<M, N, T> &rhs);
   
   
   //@}-----------------------------------------------------------------
   ///@name Scalar Mutator Operators
   //@{-----------------------------------------------------------------
   
   inline       Matrix<M, N, T> &operator*=(const T &scale);
   inline       Matrix<M, N, T> &operator/=(const T &scale);
   
   
   //@}-----------------------------------------------------------------
   ///@name Arithmetic Operators
   //@{-----------------------------------------------------------------
   
   inline       Matrix<M, N, T>  operator+ (const Matrix<M, N, T> &rhs) const;
   inline       Matrix<M, N, T>  operator- (const Matrix<M, N, T> &rhs) const;
   
   /// MxN (square) matrix * MxN (square) matrix yields an MxN (square) matrix
   inline       Matrix<M, N, T>  operator* (const Matrix<M, N, T> &rhs) const;
   
   /// MxN matrix * N-length column vector yields an M-length vector
   /// @note this method returns an M-length Vector. Make sure you understand why.
   inline       Point<M, T>      operator* (const Point<N, T> &rhs) const;
   inline       Vector<M, T>     operator* (const Vector<N, T> &rhs) const;
   inline       Vector<M - 1, T> operator* (const Vector<N - 1, T> &rhs) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name Scalar Arithmetic Operators
   //@{-----------------------------------------------------------------
   
   inline       Matrix<M, N, T>  operator* (const T &scale) const;
   inline       Matrix<M, N, T>  operator/ (const T &scale) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name More Complex Functionality
   //@{-----------------------------------------------------------------
   
   /// @returns whether or not this Matrix is diagonal
   inline bool isDiagonal() const;
   
   /// @returns a Vector containing the index'th row of this matrix
   /// @note Modifying the Vector returned will directly affect this Matrix's data
   inline Vector<N, T> &row(unsigned index);
   
   /// @returns a Vector containing the index'th row of this matrix
   /// @note Modifying the Vector returned will Not affect this Matrix's data
   inline Vector<N, T>  row(unsigned index) const;
   
   /// @returns a Vector containing the index'th column of this matrix
   /// @note Modifying the Vector returned will Not affect this Matrix's data
   inline Vector<M, T>  col(unsigned index) const;
   
   /// Sets the index'th column of this matrix to the given Vector
   inline Matrix<M, N, T> &setCol(unsigned index,
                                  const Vector<M, T> &vec);
   
   /// @returns the transpose of this matrix
   const Matrix<M, N, T> getTranspose() const;
   
   /**
    * Fills the data provided with the transpose of this matrix (since OpenGL 
    * matrices are stored in column-major format).
    */
   void fillGLMatrix(T *bufferToFill) const;
   
   /**
    * @returns true if this matrix is strictly upper-triangular. An upper-
    *    triangular matrix contains only zeros below its diagonal (but may 
    *    contain non-zero values on and above its diagonal).
    * 
    * Example 4x4 upper triangular matrix:
    *    [ 1 2 3 4 ] 
    *    [ 0 5 6 7 ]
    *    [ 0 0 0 8 ] 
    *    [ 0 0 0 9 ]
    *
    * Example 4x4 non upper triangular matrix:
    *    [ 1 2 3 4 ]
    *    [ 0 5 6 7 ]
    *    [ 8 0 0 9 ]
    *    [ 0 0 0 0 ]
    */
   bool isUpperTriangular() const;
   
   /**
    * @returns true if this matrix is strictly lower-triangular. An lower-
    *    triangular matrix contains only zeros above its diagonal (but may 
    *    contain non-zero values on and below its diagonal).
    * 
    * Example 4x4  triangular matrix:
    *    [ 1 0 0 0 ] 
    *    [ 2 3 0 0 ]
    *    [ 4 5 0 0 ] 
    *    [ 6 7 8 9 ]
    *
    * Example 4x4 non upper triangular matrix:
    *    [ 1 0 0 2 ]
    *    [ 3 4 0 5 ]
    *    [ 6 7 8 0 ]
    *    [ 0 0 0 0 ]
    */
   bool isLowerTriangular() const;
   
   /// @returns the inverse of this matrix, assuming it exists
   Matrix<M, N, T> getInverse() const;
   
   /// @returns the determinant of this matrix
   T getDeterminant() const;
   
   /// Cleans up a matrix (0's out entries that are less than epsilon)
   void cleanup();
   
   /// Convenience Constructor
   explicit Matrix(const T &v0,      const T &v1 = 0,  const T &v2 = 0, 
                   const T &v3 = 0,  const T &v4 = 0,  const T &v5 = 0, 
                   const T &v6 = 0,  const T &v7 = 0,  const T &v8 = 0, 
                   const T &v9 = 0,  const T &v10 = 0, const T &v11 = 0, 
                   const T &v12 = 0, const T &v13 = 0, const T &v14 = 0, 
                   const T &v15 = 0);

   //@}-----------------------------------------------------------------
};


/// Standard 4x4 matrix of real_ts
typedef Matrix<4, 4> Matrix4x4;

/// Standard 3x3 matrix of real_ts
typedef Matrix<3, 3> Matrix3x3;

/// Standard 2x2 matrix of real_ts
typedef Matrix<2, 2> Matrix2x2;


///@name Extra operators where Matrix is on right-hand side
//@{-----------------------------------------------------------------------------

/// 1xM row vector * MxN matrix yields a 1xN vector
template <unsigned M, unsigned N, typename T>
MILTON_DLL_EXPORT
   inline Vector<N, T>    operator* (const Vector<M, T> &lhs, const Matrix<M, N, T> &rhs);

/// @returns the MxN matrix resulting from multiplying a scalar by an MxN matrix
template <unsigned M, unsigned N, typename T>
MILTON_DLL_EXPORT
   inline Matrix<M, N, T> operator* (const T &scale, const Matrix<M, N, T> &rhs);

/// Prints a Matrix to an output stream
template <unsigned M, unsigned N, typename T>
MILTON_DLL_EXPORT
   inline std::ostream        &operator<<(std::ostream &os, const Matrix<M, N, T> &m);

//@}-----------------------------------------------------------------------------

}

/* Include inline implementations */
#include <common/math/Matrix.inl>

namespace milton {

/* The following global functions are not templated, so they will be found in 
 * Matrix.cpp
 */

///@name Routines which generate specific-purpose transformation matrices
//@{-----------------------------------------------------------------------------

/// @returns the scale matrix described by the vector
extern MILTON_DLL_EXPORT Matrix4x4 getScaleMat(const Vector3 &v);
/// @returns the translation matrix described by the vector
extern MILTON_DLL_EXPORT Matrix4x4 getTransMat(const Vector3 &v);
/// @returns the rotation matrix about the x axis by the specified angle
extern MILTON_DLL_EXPORT Matrix4x4 getRotXMat (const real_t radians);
/// @returns the rotation matrix about the y axis by the specified angle
extern MILTON_DLL_EXPORT Matrix4x4 getRotYMat (const real_t radians);
/// @returns the rotation matrix about the z axis by the specified angle
extern MILTON_DLL_EXPORT Matrix4x4 getRotZMat (const real_t radians);
/// @returns the rotation matrix around the vector and point by the specified angle
extern MILTON_DLL_EXPORT Matrix4x4 getRotMat  (const Point3 &p, const Vector3 &v, const real_t a);

//@}-----------------------------------------------------------------------------
///@name Routines which generate specific-purpose inverse transformation matrices
//@{-----------------------------------------------------------------------------

/// @returns the inverse scale matrix described by the vector
extern MILTON_DLL_EXPORT Matrix4x4 getInvScaleMat(const Vector3 &v);
/// @returns the inverse translation matrix described by the vector
extern MILTON_DLL_EXPORT Matrix4x4 getInvTransMat(const Vector3 &v);
/// @returns the inverse rotation matrix about the x axis by the specified angle
extern MILTON_DLL_EXPORT Matrix4x4 getInvRotXMat (const real_t radians);
/// @returns the inverse rotation matrix about the y axis by the specified angle
extern MILTON_DLL_EXPORT Matrix4x4 getInvRotYMat (const real_t radians);
/// @returns the inverse rotation matrix about the z axis by the specified angle
extern MILTON_DLL_EXPORT Matrix4x4 getInvRotZMat (const real_t radians);
/// @returns the inverse rotation matrix around the vector and point by the specified angle
extern MILTON_DLL_EXPORT Matrix4x4 getInvRotMat  (const Point3 &p, const Vector3 &v, const real_t a);

//@}-----------------------------------------------------------------------------

}

#endif // MATRIX_H_

