/**<!-------------------------------------------------------------------->
   @file   Point.inl
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides basic functionality for a homogenous Point
   <!-------------------------------------------------------------------->**/

#ifndef POINT_INL_
#define POINT_INL_

#include <iostream>
using namespace std;

namespace milton {


// Constructors
// ------------

// Expects N arguments of type T
// (srcData is a pointer to an N-length array of type T)
template <unsigned N, typename T>
inline Point<N, T>::Point(const T *srcData) {
   memcpy(data, srcData, POINT_SIZE);
   
   data[N - 1] = 1;
}

// Constructs an zero Point
template <unsigned N, typename T>
inline Point<N, T>::Point() {
   memset(data, 0, POINT_SIZE);
   
   data[N - 1] = 1;
}

// Copy Constructor; copies the underlying data from v to this point
template <unsigned N, typename T>
inline Point<N, T>::Point(const Point<N, T> &v) {
   memcpy(data, v.data, POINT_SIZE);
}


// Accessor Operators
// ------------------

// @returns a const reference to the element at the given index
template <unsigned N, typename T>
inline const T &Point<N, T>::operator[](const unsigned index) const {
   ASSERT(index < N);
   
   return data[index];
}

// @returns a reference to the element at the given index
// @note changes to the returned element will affect this point
template <unsigned N, typename T>
inline       T &Point<N, T>::operator[](const unsigned index) {
   ASSERT(index < N);
   
   return data[index];
}

// @returns a pointer to the underlying data (N-length array of type T)
template <unsigned N, typename T>
inline const T *Point<N, T>::operator* () const {
   return (T*)this;
}

// @returns a pointer to the underlying data (N-length array of type T)
template <unsigned N, typename T>
inline       T *Point<N, T>::operator* () {
   return (T*)this;
}


// Equality Operators
// ------------------
template <unsigned N, typename T>
inline       bool       Point<N, T>::operator==(const Point<N, T> &v) const
{
   for(unsigned i = N; i--;)
      if (NEQ(data[i], v.data[i]))
         return false;
   
   return true;
}

template <unsigned N, typename T>
inline       bool       Point<N, T>::operator!=(const Point<N, T> &v) const 
{
   return !((*this) == v);
}

// Mutator Operators
// -----------------
template <unsigned N, typename T>
inline       Point<N, T> &Point<N, T>::operator =(const Point<N, T> &v)
{
   memcpy(data, v.data, POINT_SIZE);
   
   return *this;
}

template <unsigned N, typename T>
inline       Point<N, T> &Point<N, T>::operator+=(const Vector<N - 1, T> &rhs)
{
   for(unsigned i = N - 1; i--;)
      data[i] += rhs.data[i];
   
   return *this;
}

template <unsigned N, typename T>
inline       Point<N, T> &Point<N, T>::operator-=(const Vector<N - 1, T> &rhs)
{
   return (*this += -rhs);
}


// Scalar Mutator Operators
// ------------------------
template <unsigned N, typename T>
inline       Point<N, T> &Point<N, T>::operator*=(const T &scale) {
   for(unsigned i = N; i--;)
      data[i] *= scale;
   
   return *this;
}

template <unsigned N, typename T>
inline       Point<N, T> &Point<N, T>::operator/=(const T &scale) {
   if (NEQ(static_cast<T>(0), scale)) {
      for(unsigned i = N; i--;)
         data[i] /= scale;
   } else {
      cerr << "Error: Attempting to divide Point by zero" << endl << endl;
   }
   
   return *this;
}


// Arithmetic Operators
// --------------------

template <unsigned N, typename T>
inline       Point<N, T>  Point<N, T>::operator+ (const Vector<N - 1, T> &rhs) const
{
   T d[POINT_NO_ELEMENTS];
   
   d[N - 1] = data[N - 1];
   for(unsigned i = N - 1; i--;)
      d[i] = data[i] + rhs.data[i];
   
   return Point<N, T>(d);
}

template <unsigned N, typename T>
inline       Point<N, T>  Point<N, T>::operator- (const Vector<N - 1, T> &rhs) const
{
   T d[POINT_NO_ELEMENTS];
   
   d[N - 1] = data[N - 1];
   for(unsigned i = N - 1; i--;)
      d[i] = data[i] - rhs.data[i];
   
   return Point<N, T>(d);
}

template <unsigned N, typename T>
inline       Vector<N - 1, T>  Point<N, T>::operator- (const Point<N, T> &rhs) const
{
   T d[POINT_NO_ELEMENTS];
   
   for(unsigned i = N - 1; i--;)
      d[i] = data[i] - rhs.data[i];
   
   return Vector<N - 1, T>(d);
}

// Scalar Arithmetic Operators
template <unsigned N, typename T>
inline       Point<N, T>  Point<N, T>::operator* (const T &scale) const {
   T d[POINT_NO_ELEMENTS];
   
   for(unsigned i = N; i--;)
      d[i] = data[i] * scale;
   
   return Point<N, T>(d);
}

template <unsigned N, typename T>
inline       Point<N, T>  Point<N, T>::operator/ (const T &scale) const {
   if (EQ(static_cast<T>(0), scale)) {
      cerr << "Error: Attempting to divide Point by zero" << endl << endl;
      
      return Point<N, T>();
   } else {
      T d[POINT_NO_ELEMENTS];
      
      for(unsigned i = N; i--;)
         d[i] = data[i] / scale;
      
      return Point<N, T>(d);
   }
}


// More Complex Functionality
// --------------------------

// @returns whether or not this Point is the zero point
template <unsigned N, typename T>
inline bool Point<N, T>::isZero() const {
   for(unsigned i = N; i--;) {
      if (data[i] != 0)
         return false;
   }
   
   return true;
}

// @returns the magnitude of the vector connecting this point to the one 
//    passed in
template <unsigned N, typename T>
inline T Point<N, T>::getDistance(const Point<N, T> &v) const {
   return sqrt(getDistance2(v));
}

// @returns the squared magnitude of the point connecting this point to 
//    the one passed in
template <unsigned N, typename T>
inline T Point<N, T>::getDistance2(const Point<N, T> &v) const {
   return (*this - v).getMagnitude2();
}

// @returns the sum of the components in this Point
template <unsigned N, typename T>
inline T Point<N, T>::getSum() const {
   T sum = 0;
   
   for(unsigned i = N; i--;)
      sum += data[i];
   
   return sum;
}

// @returns the average of the components in this Point
template <unsigned N, typename T>
inline T Point<N, T>::getAverage() const {
   return (getSum() / N);
}


// Specialized Point Functionality
// --------------------------------

// Convenience Constructor
template <unsigned N, typename T>
inline Point<N, T>::Point(const T &v0, const T &v1, const T &v2, 
                          const T &v3)
{
   // Already filled in for you since this is a convenience constructor only
   
   if (N > 0)
      data[0] = v0;
   if (N > 1)
      data[1] = v1;
   if (N > 2)
      data[2] = v2;
   if (N > 3)
      data[3] = v3;
}


// Extra operators where Point is on right-hand side
// --------------------------------------------------

// @returns the N-length point resulting from multiplying a scalar by an 
//    N-length point
template <unsigned N, typename T>
inline       Point<N, T>  operator* (const T &scale, 
                                     const Point<N, T> &rhs)
{
   return rhs * scale;
}

// @returns (-1) * rhs, which is a negated version of the original right 
//    hand side point
template <unsigned N, typename T>
inline       Point<N, T> operator- (const Point<N, T> &rhs)
{
   return rhs * (-1);
}

// Prints a Point to an output stream
template <unsigned N, typename T>
inline       std::ostream      &operator<<(std::ostream &os, 
                                           const Point<N, T> &v)
{
   os << "[ ";
   
   for(unsigned i = 0; i < N; ++i)
      os << v.data[i] << (i < N - 1 ? ", " : "");

   os << " ]";
   return os;
}


#if (!defined(MILTON_ARCH_WINDOWS) || defined(MILTON_EXPORTS))

// Cleans up point (0's out entries that are less than epsilon)
template <unsigned N, typename T>
void Point<N, T>::cleanup() {
   for(unsigned i = N; i--;) {
      if (EQ(data[i], static_cast<T>(0))) {
         data[i] = 0;
      }
   }
}

#endif

}

#endif // POINT_INL_

