/**<!-------------------------------------------------------------------->
   @file   Matrix.inl
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

#ifndef MATRIX_INL_
#define MATRIX_INL_

#include <iostream>
using namespace std;

namespace milton {


// Constructors
// ------------

// Expects arguments in row-major order
// (srcData is a pointer to an M*N-length array of type T)
template <unsigned M, unsigned N, typename T>
inline Matrix<M, N, T>::Matrix(const T *srcData) {
   memcpy(rows, srcData, MATRIX_SIZE);
}

// Constructs an zero Matrix
template <unsigned M, unsigned N, typename T>
inline Matrix<M, N, T>::Matrix() {
   memset(rows, 0, MATRIX_SIZE);
}

// Copy Constructor; should copy the underlying data from m to this matrix
template <unsigned M, unsigned N, typename T>
inline Matrix<M, N, T>::Matrix(const Matrix &m) {
   memcpy(rows, m.rows, MATRIX_SIZE);
}


// Accessor Operators
// ------------------

// @returns a const reference to the row at the given index
template <unsigned M, unsigned N, typename T>
inline const Vector<N, T>   &Matrix<M, N, T>::operator[](const unsigned rowIndex) const
{
   ASSERT(rowIndex < M);
   return rows[rowIndex];
}

// @returns a reference to the row at the given index
// @note changes to the returned vector will affect this matrix
template <unsigned M, unsigned N, typename T>
inline       Vector<N, T>   &Matrix<M, N, T>::operator[](const unsigned rowIndex)
{
   ASSERT(rowIndex < M);
   return rows[rowIndex];
}

// @returns a const pointer to the raw, underlying data
//    (row-major, M*N-length array of type T)
template <unsigned M, unsigned N, typename T>
inline const T    *Matrix<M, N, T>::operator* () const {
   return (T*)this;
}

// @returns a pointer to the raw, underlying data
//    (row-major, M*N-length array of type T)
template <unsigned M, unsigned N, typename T>
inline       T    *Matrix<M, N, T>::operator* () {
   return (T*)this;
}
 
   
// Equality Operators
// ------------------


// Evaluates whether or not this matrix is the same as the matrix m.
// This is an example of operator overloading in which the Matrix class
// _overloads_ the "==" operator.  Consider the following code snippet:
//    Matrix a,b;
//    ...
//    bool matrixEquality = (a == b);
// Then (a == b) calls this method on the object a with the argument b.
template <unsigned M, unsigned N, typename T>
inline       bool       Matrix<M, N, T>::operator==(const Matrix<M, N, T> &m) const
{
   for(unsigned i = M; i--;) {
      if ((*this)[i] != m[i])
         return false;
   }
   
   return true;
}

// Evaluates whether or not this matrix is not equal to the matrix m.
template <unsigned M, unsigned N, typename T>
inline       bool       Matrix<M, N, T>::operator!=(const Matrix<M, N, T> &m) const
{
   return !((*this) == m);
}


// Mutator Operators
// -----------------

// Assigns the contents of this matrix to the contents of matrix m. This function is executed
// when the equals operator is used to assign a Matrix<M,N,T> to a Matrix<M,N,T>
template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T> &Matrix<M, N, T>::operator =(const Matrix<M, N, T> &m)
{
   memcpy(rows, m.rows, MATRIX_SIZE);
   
   return *this;
}

template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T> &Matrix<M, N, T>::operator+=(const Matrix<M, N, T> &rhs)
{
   for(unsigned i = M; i--;)
      rows[i] += rhs[i];
   
   return *this;
}

template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T> &Matrix<M, N, T>::operator-=(const Matrix<M, N, T> &rhs)
{
   for(unsigned i = M; i--;)
      rows[i] -= rhs[i];
   
   return *this;
}

template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T> &Matrix<M, N, T>::operator*=(const Matrix<M, N, T> &rhs)
{
   T d[M * N];
   
   for(unsigned i = M * N; i--;)
      d[i] = row(i / N).dot(rhs.col(i % N));
   
   memcpy(rows, d, MATRIX_SIZE);
   return *this;
}


// Scalar Mutator Operators
template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T> &Matrix<M, N, T>::operator*=(const T &scale)
{
   for(unsigned i = M; i--;)
      rows[i] *= scale;
   
   return *this;
}

template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T> &Matrix<M, N, T>::operator/=(const T &scale)
{
   ASSERT(NEQ(scale, static_cast<T>(0)));
   
   for(unsigned i = M; i--;)
      rows[i] /= scale;
   
   return *this;
}


// Arithmetic Operators
// -----------------
template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T>  Matrix<M, N, T>::operator+ (const Matrix<M, N, T> &rhs) const
{
   Matrix<M, N, T> ret;
   
   for(unsigned i = M; i--;)
      ret[i] = rows[i] + rhs[i];
   
   return ret;
}

template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T>  Matrix<M, N, T>::operator- (const Matrix<M, N, T> &rhs) const
{
   Matrix<M, N, T> ret;
   
   for(unsigned i = M; i--;)
      ret[i] = rows[i] - rhs[i];
   
   return ret;
}

// MxN (square) matrix * MxN (square) matrix yields an MxN (square) matrix
template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T>  Matrix<M, N, T>::operator* (const Matrix<M, N, T> &rhs) const
{
   Matrix<M, N, T> ret = (*this);
   
   ret *= rhs;
   return ret;
}

// MxN matrix * N-length column vector yields an M-length vector
// @note this method returns an M-length Vector. Make sure you understand why.
template <unsigned M, unsigned N, typename T>
inline       Point<M, T>     Matrix<M, N, T>::operator* (const Point<N, T> &rhs) const
{
   Point<M, T> ret;
   ret[M - 1] = 0;
   
   for(unsigned i = M; i--;)
      for(unsigned j = N; j--;)
         ret[i] += rows[i][j] * rhs[j];
   
   return ret;
}

// MxN matrix * N-length column vector yields an M-length vector
// @note this method returns an M-length Vector. Make sure you understand why.
template <unsigned M, unsigned N, typename T>
inline       Vector<M, T>     Matrix<M, N, T>::operator* (const Vector<N, T> &rhs) const
{
   Vector<M, T> ret;
   
   for(unsigned i = M; i--;)
      ret[i] = rows[i].dot(rhs);
   
   return ret;
}

template <unsigned M, unsigned N, typename T>
inline       Vector<M - 1, T>     Matrix<M, N, T>::operator* (const Vector<N - 1, T> &rhs) const
{
   Vector<M - 1, T> ret;
   
   for(unsigned i = M - 1; i--;)
      for(unsigned j = N - 1; j--;)
         ret[i] += rows[i][j] * rhs[j];
   
   return ret;
}


// Scalar Arithmetic Operators
template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T>  Matrix<M, N, T>::operator* (const T &scale) const
{
   Matrix<M, N, T> ret;
   
   for(unsigned i = M; i--;)
      ret[i] = rows[i] * scale;
   
   return ret;
}

template <unsigned M, unsigned N, typename T>
inline       Matrix<M, N, T>  Matrix<M, N, T>::operator/ (const T &scale) const
{
   Matrix<M, N, T> ret;
   ASSERT(NEQ(scale, static_cast<T>(0)));
   
   for(unsigned i = M; i--;)
      ret[i] = rows[i] / scale;
   
   return ret;
}


// More Complex Functionality
// --------------------------

// @returns whether or not this Matrix is diagonal
template <unsigned M, unsigned N, typename T>
inline bool Matrix<M, N, T>::isDiagonal() const {
   for(unsigned i = M; i--;)
      for(unsigned j = N; j--;)
         if (i != j && NEQ((*this)[i][j], static_cast<T>(0)))
            return false;
   
   return true;
}

// @returns a Vector containing the index'th row of this matrix
// Modifying the Vector returned will directly affect this Matrix's data
template <unsigned M, unsigned N, typename T>
inline Vector<N, T> &Matrix<M, N, T>::row(unsigned index) {
   ASSERT(index < M);
   
   return rows[index];
}

// @returns a Vector containing the index'th row of this matrix
// Modifying the Vector returned will Not affect this Matrix's data
template <unsigned M, unsigned N, typename T>
inline Vector<N, T>  Matrix<M, N, T>::row(unsigned index) const {
   ASSERT(index < M);
   
   return Vector<N, T>(rows[index]);
}

// @returns a Vector containing the index'th column of this matrix
// Modifying the Vector returned will Not affect this Matrix's data
template <unsigned M, unsigned N, typename T>
inline Vector<M, T>  Matrix<M, N, T>::col(unsigned index) const {
   ASSERT(index < N);
   T temp[M];

   for(unsigned i = M; i--;)
      temp[i] = rows[i][index];
   
   return Vector<M, T>(temp);
}

// Sets the index'th column of this matrix to the given Vector
template <unsigned M, unsigned N, typename T>
inline Matrix<M, N, T> &Matrix<M, N, T>::setCol(
      unsigned index, const Vector<M, T> &vec)
{
   ASSERT(index < N);
   
   for(unsigned i = M; i--;)
      rows[i][index] = vec[i];
   
   return (*this);
}


// Extra operators where Matrix is on right-hand side
// -----------------------------------------------------

// 1xM row vector * MxN matrix yields a 1xN vector
template <unsigned M, unsigned N, typename T>
inline Vector<N, T> operator* (const Vector<M, T> &lhs, const Matrix<M, N, T> &rhs)
{
   T temp[N];
   
   for(unsigned j = N; j--;)
      temp[j] = lhs.dot(rhs.col(j));
   
   return Vector<N, T>(temp);
}

// @returns the MxN matrix resulting from multiplying a scalar by an MxN matrix
template <unsigned M, unsigned N, typename T>
inline Matrix<M, N, T> operator* (const T &scale, const Matrix<M, N, T> &rhs)
{
   return rhs * scale;
}

// Prints a Matrix to an output stream
template <unsigned M, unsigned N, typename T>
inline std::ostream &operator<<(std::ostream &os, const Matrix<M, N, T> &m)
{
   os << "[ ";
   
   for(unsigned i = 0; i < M; ++i)
      os << m[i] << (i < M - 1 ? " " : "");
   
   os << " ]";
   
   return os;
}


#if (!defined(MILTON_ARCH_WINDOWS) || defined(MILTON_EXPORTS))

// @returns the transpose of this matrix
template <unsigned M, unsigned N, typename T>
const Matrix<M, N, T> Matrix<M, N, T>::getTranspose() const {
   ASSERT(M == N);
   T temp[M * N];
   
   for(unsigned i = M; i--;)
      for(unsigned j = N; j--;)
         temp[i * N + j] = (*this)[j][i];
   
   return Matrix<M, N, T>(temp);
}

/*
 * Fills the data provided with the transpose of this matrix (since OpenGL 
 * matrices are stored in column-major format).
 */
template <unsigned M, unsigned N, typename T>
void Matrix<M, N, T>::fillGLMatrix(T *bufferToFill) const {
   ASSERT(M == N && M == 4);
   
   for(unsigned i = M; i--;)
      for(unsigned j = N; j--;)
         bufferToFill[i * N + j] = (*this)[j][i];
}

/*
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
template <unsigned M, unsigned N, typename T>
bool Matrix<M, N, T>::isUpperTriangular() const {
   for(unsigned i = M; i--;)
      for(unsigned j = i; j--;)
         if (NEQ((*this)[i][j], static_cast<T>(0)))
            return false;
   
   return true;
}

/*
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
template <unsigned M, unsigned N, typename T>
bool Matrix<M, N, T>::isLowerTriangular() const {
   for(unsigned i = M; i--;)
      for(unsigned j = i + 1; j < N; ++j)
         if (NEQ((*this)[i][j], static_cast<T>(0)))
            return false;
   
   return true;
}

// @returns the inverse of this matrix, assuming it exists
template <unsigned M, unsigned N, typename T>
Matrix<M, N, T> Matrix<M, N, T>::getInverse() const {
   const T &det = getDeterminant();
   
   if (EQ(det, static_cast<T>(0))) {
      printf("Matrix not invertible. Returning the identity matrix.\n");
      return Matrix<M, N, T>();
   }
   
   const real_t &invDet = static_cast<T>(1) / det;
   
   // Ensure matrix is not singular
   if (M == N && M == 3) {
      return invDet * Matrix<M, N, T>(
         // Column 1
         ((*this)[2][2] * (*this)[1][1] - (*this)[2][1] * (*this)[1][2]), 
         -((*this)[2][2] * (*this)[0][1] - (*this)[2][1] * (*this)[0][2]), 
         ((*this)[1][2] * (*this)[0][1] - (*this)[1][1] * (*this)[0][2]), 
         
         // Column 2
         -((*this)[2][2] * (*this)[1][0] - (*this)[2][0] * (*this)[1][2]), 
         ((*this)[2][2] * (*this)[0][0] - (*this)[2][0] * (*this)[0][2]), 
         -((*this)[1][2] * (*this)[0][0] - (*this)[1][0] * (*this)[0][2]), 
         
         // Column 3
         ((*this)[2][1] * (*this)[1][0] - (*this)[2][0] * (*this)[1][1]), 
         -((*this)[2][1] * (*this)[0][0] - (*this)[2][0] * (*this)[0][1]), 
         ((*this)[1][1] * (*this)[0][0] - (*this)[1][0] * (*this)[0][1]));
   } else if (N == 4) {
      const T *data = (**this);
      
      const T &a = data[0];
      const T &b = data[1];
      const T &c = data[2];
      const T &d = data[3];
      const T &e = data[4];
      const T &f = data[5];
      const T &g = data[6];
      const T &h = data[7];
      const T &i = data[8];
      const T &j = data[9];
      const T &k = data[10];
      const T &l = data[11];
      const T &m = data[12];
      const T &n = data[13];
      const T &o = data[14];
      const T &p = data[15];
      
      return Matrix<M, N, T>(
         (-h*k*n + g*l*n + h*j*o - f*l*o - g*j*p + f*k*p) * invDet, 
         (d*k*n - c*l*n - d*j*o + b*l*o + c*j*p - b*k*p) * invDet, 
         (-d*g*n + c*h*n + d*f*o - b*h*o - c*f*p + b*g*p) * invDet, 
         (d*g*j - c*h*j - d*f*k + b*h*k + c*f*l - b*g*l) * invDet, 
         
         (h*k*m - g*l*m - h*i*o + e*l*o + g*i*p - e*k*p) * invDet, 
         (-d*k*m + c*l*m + d*i*o - a*l*o - c*i*p + a*k*p) * invDet, 
         (d*g*m - c*h*m - d*e*o + a*h*o + c*e*p - a*g*p) * invDet, 
         (-d*g*i + c*h*i + d*e*k - a*h*k - c*e*l + a*g*l) * invDet, 
         
         (-h*j*m + f*l*m + h*i*n - e*l*n - f*i*p + e*j*p) * invDet, 
         (d*j*m - b*l*m - d*i*n + a*l*n + b*i*p - a*j*p) * invDet, 
         (-d*f*m + b*h*m + d*e*n - a*h*n - b*e*p + a*f*p) * invDet, 
         (d*f*i - b*h*i - d*e*j + a*h*j + b*e*l - a*f*l) * invDet, 
         
         (g*j*m - f*k*m - g*i*n + e*k*n + f*i*o - e*j*o) * invDet, 
         (-c*j*m + b*k*m + c*i*n - a*k*n - b*i*o + a*j*o) * invDet, 
         (c*f*m - b*g*m - c*e*n + a*g*n + b*e*o - a*f*o) * invDet, 
         (-c*f*i + b*g*i + c*e*j - a*g*j - b*e*k + a*f*k) * invDet);
   }
   
   // TODO!!!
   ASSERT(0 && "Not yet Implemented"); // Matrix::getInverse for N != 3 or 4
}

// @returns the determinant of this matrix
template <unsigned M, unsigned N, typename T>
T Matrix<M, N, T>::getDeterminant() const {
   ASSERT(M == N && "Matrix::getDeterminant() is only defined for square matrices!\n");
   
   if (M == 3) {
      // (row-major indexing)
      return 
         (*this)[0][0] * ((*this)[1][1] * (*this)[2][2] - (*this)[1][2] * (*this)[2][1]) + 
        -(*this)[0][1] * ((*this)[1][0] * (*this)[2][2] - (*this)[1][2] * (*this)[2][0]) + 
         (*this)[0][2] * ((*this)[1][0] * (*this)[2][1] - (*this)[1][1] * (*this)[2][0]);
   } else if (M == 4) {
      const T *data = (**this);
      
      const T &a = data[0];
      const T &b = data[1];
      const T &c = data[2];
      const T &d = data[3];
      const T &e = data[4];
      const T &f = data[5];
      const T &g = data[6];
      const T &h = data[7];
      const T &i = data[8];
      const T &j = data[9];
      const T &k = data[10];
      const T &l = data[11];
      const T &m = data[12];
      const T &n = data[13];
      const T &o = data[14];
      const T &p = data[15];
      
      return 
         d*g*j*m - c*h*j*m - d*f*k*m + b*h*k*m + c*f*l*m - b*g*l*m - d*g*i*n + 
         c*h*i*n + d*e*k*n - a*h*k*n - c*e*l*n + a*g*l*n + d*f*i*o - b*h*i*o - 
         d*e*j*o + a*h*j*o + b*e*l*o - a*f*l*o - c*f*i*p + b*g*i*p + c*e*j*p - 
         a*g*j*p - b*e*k*p + a*f*k*p;
   }
   
   ASSERT(0 && "Not yet Implemented"); // Matrix::getInverse for N != 3 or 4
}

// Cleans up a matrix (0's out entries that are less than epsilon)
template <unsigned M, unsigned N, typename T>
void Matrix<M, N, T>::cleanup() {
   for (unsigned i = M; i --;)
      rows[i].cleanup();
}

// Convenience Constructor
template <unsigned M, unsigned N, typename T>
Matrix<M, N, T>::Matrix(const T &v0,  const T &v1,  const T &v2, 
                                  const T &v3,  const T &v4,  const T &v5, 
                                  const T &v6,  const T &v7,  const T &v8, 
                                  const T &v9,  const T &v10, const T &v11, 
                                  const T &v12, const T &v13, const T &v14, 
                                  const T &v15)
{
   if (M == 4 && N == 4) {
      (*this)[0][0] = v0;
      (*this)[0][1] = v1;
      (*this)[0][2] = v2;
      (*this)[0][3] = v3;
      (*this)[1][0] = v4;
      (*this)[1][1] = v5;
      (*this)[1][2] = v6;
      (*this)[1][3] = v7;
      (*this)[2][0] = v8;
      (*this)[2][1] = v9;
      (*this)[2][2] = v10;
      (*this)[2][3] = v11;
      (*this)[3][0] = v12;
      (*this)[3][1] = v13;
      (*this)[3][2] = v14;
      (*this)[3][3] = v15;
   } else if (M == 3 && N == 3) {
      (*this)[0][0] = v0;
      (*this)[0][1] = v1;
      (*this)[0][2] = v2;
      (*this)[1][0] = v3;
      (*this)[1][1] = v4;
      (*this)[1][2] = v5;
      (*this)[2][0] = v6;
      (*this)[2][1] = v7;
      (*this)[2][2] = v8;
   } else {
      // TODO (optional) handle other cases
      ASSERT(0 && "Not yet implemented: Convenience constructor is only "
                  "currently implemented for 3x3 and 4x4 matrices\n");
   }
}

#endif

}

#endif // MATRIX_INL_

