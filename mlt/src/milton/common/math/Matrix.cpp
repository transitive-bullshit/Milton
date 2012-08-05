/**<!-------------------------------------------------------------------->
   @file   Matrix.cpp
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

#include "Matrix.h"
#include <iostream>

namespace milton {


//@name Routines which generate specific-purpose transformation matrices
//@{---------------------------------------------------------------------

// @returns the scale matrix described by the vector
Matrix4x4 getScaleMat(const Vector3 &v) {
   return Matrix4x4(v[0], 0, 0, 0, 
                    0, v[1], 0, 0, 
                    0, 0, v[2], 0, 
                    0, 0, 0, 1);
}

// @returns the translation matrix described by the vector
Matrix4x4 getTransMat(const Vector3 &v) {
   return Matrix4x4(1, 0, 0, v[0], 
                    0, 1, 0, v[1], 
                    0, 0, 1, v[2], 
                    0, 0, 0, 1);
}

// @returns the rotation matrix about the x axis by the specified angle
Matrix4x4 getRotXMat (const real_t radians) {
   const real_t cosTheta = cos(radians);
   const real_t sinTheta = sin(radians);
   
   return Matrix4x4(1, 0, 0, 0, 
                    0, cosTheta, -sinTheta, 0, 
                    0, sinTheta, cosTheta, 0, 
                    0, 0, 0, 1);
}

// @returns the rotation matrix about the y axis by the specified angle
Matrix4x4 getRotYMat (const real_t radians) {
   const real_t cosTheta = cos(radians);
   const real_t sinTheta = sin(radians);

   return Matrix4x4(cosTheta, 0, sinTheta, 0, 
                    0, 1, 0, 0, 
                    -sinTheta, 0, cosTheta, 0, 
                    0, 0, 0, 1);
}

// @returns the rotation matrix about the z axis by the specified angle
Matrix4x4 getRotZMat (const real_t radians) {
   const real_t cosTheta = cos(radians);
   const real_t sinTheta = sin(radians);

   return Matrix4x4(cosTheta, -sinTheta, 0, 0, 
                    sinTheta, cosTheta, 0, 0, 
                    0, 0, 1, 0, 
                    0, 0, 0, 1);
}

// @returns the rotation matrix around the vector and point by the specified angle
Matrix4x4 getRotMat  (const Point3 &p, const Vector3 &v, const real_t a) {
   // Translate to origin from point p
   const real_t vZ = v[2];
   const real_t vX = v[0];
   const real_t theta = atan2(vZ, vX);
   const real_t phi   = -atan2(v[1], sqrt(vX * vX + vZ * vZ));
   
   const Matrix4x4 &transToOrigin = getInvTransMat(Vector3(p[0], p[1], p[2]));
   const Matrix4x4 &A = getRotYMat(theta);
   const Matrix4x4 &B = getRotZMat(phi);
   const Matrix4x4 &C = getRotXMat(a);
   const Matrix4x4 &invA = getInvRotYMat(theta);
   const Matrix4x4 &invB = getInvRotZMat(phi);
   const Matrix4x4 &transBack = getTransMat(Vector3(p[0], p[1], p[2]));
   
   return transBack * invA * invB * C * B * A * transToOrigin;
}


// @returns the inverse scale matrix described by the vector
Matrix4x4 getInvScaleMat(const Vector3 &v) {
   if (v[0] != 0 && v[1] != 0 && v[2] != 0) {
      return Matrix4x4(1.0f / v[0], 0, 0, 0, 
                       0, 1.0f / v[1], 0, 0, 
                       0, 0, 1.0f / v[2], 0, 
                       0, 0, 0, 1);
   }

   cerr << "\nError: " << "Divide by zero in getInvScaleMat()" << endl << endl; 
   return Matrix4x4();
}

// @returns the inverse translation matrix described by the vector
Matrix4x4 getInvTransMat(const Vector3 &v) {
   return Matrix4x4(1, 0, 0, -v[0], 
                    0, 1, 0, -v[1], 
                    0, 0, 1, -v[2], 
                    0, 0, 0, 1);
}

// @returns the inverse rotation matrix about the x axis by the specified angle
Matrix4x4 getInvRotXMat (const real_t radians) {
   const real_t cosTheta = cos(radians);
   const real_t sinTheta = sin(radians);

   return Matrix4x4(1, 0, 0, 0, 
                    0, cosTheta, sinTheta, 0, 
                    0, -sinTheta, cosTheta, 0, 
                    0, 0, 0, 1);
}

// @returns the inverse rotation matrix about the y axis by the specified angle
Matrix4x4 getInvRotYMat (const real_t radians) {
   const real_t cosTheta = cos(radians);
   const real_t sinTheta = sin(radians);

   return Matrix4x4(cosTheta, 0, -sinTheta, 0, 
                    0, 1, 0, 0, 
                    sinTheta, 0, cosTheta, 0, 
                    0, 0, 0, 1);
}

// @returns the inverse rotation matrix about the z axis by the specified angle
Matrix4x4 getInvRotZMat (const real_t radians) {
   const real_t cosTheta = cos(radians);
   const real_t sinTheta = sin(radians);

   return Matrix4x4(cosTheta, sinTheta, 0, 0, 
                    -sinTheta, cosTheta, 0, 0, 
                    0, 0, 1, 0, 
                    0, 0, 0, 1);
}

// @returns the inverse rotation matrix around the vector and point by the specified angle
Matrix4x4 getInvRotMat  (const Point3 &p, const Vector3 &v, const real_t a) {
   const real_t vZ = v[2];
   const real_t vX = v[0];
   const real_t theta = atan2(vZ, vX);
   const real_t phi   = -atan2(v[1], sqrt(vX * vX + vZ * vZ));
   
   const Matrix4x4 &transToOrigin = getInvTransMat(Vector3(p[0], p[1], p[2]));
   const Matrix4x4 &A = getRotYMat(theta);
   const Matrix4x4 &B = getRotZMat(phi);
   const Matrix4x4 &C = getRotXMat(a);
   const Matrix4x4 &invA = getInvRotYMat(theta);
   const Matrix4x4 &invB = getInvRotZMat(phi);
   const Matrix4x4 &transBack = getTransMat(Vector3(p[0], p[1], p[2]));
   
   return transBack * (invA * invB * C * B * A).getTranspose() * transToOrigin;
}

//@}---------------------------------------------------------------------

// force explicit template specialization
template struct Matrix<4, 4, real_t>;

}

