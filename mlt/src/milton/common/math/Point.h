/**<!-------------------------------------------------------------------->
   @class  Point
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides basic functionality for a homogeneous point
   <!-------------------------------------------------------------------->**/

#ifndef POINT_H_
#define POINT_H_

#include <common/math/Vector.h>
#include <ostream>
#include <limits>

#define POINT_NO_ELEMENTS    (N)
#define POINT_SIZE           (sizeof(T) * POINT_NO_ELEMENTS)

namespace milton {

/**
 * @brief
 *    A Point is templated to store N elements of data of type T.  If 
 * left off, T will default to a "real_t" data type.
 * 
 * Included in this definition are typedefs for the most commonly used 
 * points (Point4, Point3, etc), and these can be thought of as shortcuts 
 * to reference their associated Point templates.  For example, 
 * Point4 can be used to refer to Point<4, real_t>.
 * 
 * A Point stores its data internally as an N-length array of type T.
 */
template <unsigned N = 4, typename T = real_t>
struct MILTON_DLL_EXPORT Point {
   
   /// Underlying data stored as an N-length array of type T
   T data[POINT_NO_ELEMENTS];
   
   
   ///@name Constructors
   //@{-----------------------------------------------------------------
   
   /// Expects N arguments of type T
   /// @note explicitly sets the homogeneous coordinate to 1 (w=1)
   inline Point(const T *srcData);
   
   /// Constructs a point at the origin (w=1)
   inline Point();
   
   /// Copy Constructor
   inline Point(const Point<N, T> &v);
   
   
   //@}-----------------------------------------------------------------
   ///@name Static convenience constructors to generate common points
   //@{-----------------------------------------------------------------
   
   /// Generates a point full of zeroes
   static inline Point<N, T> origin() {
      return Point<N, T>();
   }
   
   /// Generates a point filled with the specified value
   static inline Point<N, T> fill(const T &val) {
      Point<N, T> ret = Point<N, T>();
      
      for(unsigned i = N; i--;)
         ret[i] = val;
      
      return ret;
   }
   
   /// @returns the element-wise minimum point from the two given points
   static inline Point<N, T> min(const Point<N, T> &v1, const Point<N, T> &v2) {
      Point<N, T> ret;
      
      for(unsigned i = N; i--;)
         ret[i] = MIN(v1[i], v2[i]);

      return ret;
   }
   
   /// @returns the element-wise maximum point from the two given points
   static inline Point<N, T> max(const Point<N, T> &v1, const Point<N, T> &v2) {
      Point<N, T> ret;
      
      for(unsigned i = N; i--;)
         ret[i] = MAX(v1[i], v2[i]);

      return ret;
   }

   /// Generates a point filled with the maximum T (infinity)
   static inline Point<N, T> infinity() {
      return Point<N, T>::fill(std::numeric_limits<T>::max());
   }
   
   /// Generates a point filled with the minimum T (negative infinity)
   static inline Point<N, T> negativeInfinity() {
      return Point<N, T>::fill(std::numeric_limits<T>::min());
   }
   
   
   //@}-----------------------------------------------------------------
   ///@name Accessor Operators
   //@{-----------------------------------------------------------------
   
   /// @returns a const reference to the element at the given index
   inline const T &operator[](const unsigned index) const;
   /// @returns a       reference to the element at the given index
   /// @note changes to the returned element will affect this point
   inline       T &operator[](const unsigned index);
   
   /// @returns a pointer to the underlying data (N-length array of type T)
   inline const T *operator*() const;
   /// @returns a pointer to the underlying data (N-length array of type T)
   inline       T *operator*();
   
   
   //@}-----------------------------------------------------------------
   ///@name Equality Operators
   //@{-----------------------------------------------------------------
   
   inline       bool       operator==(const Point<N, T> &v) const;
   inline       bool       operator!=(const Point<N, T> &v) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name Mutator Operators
   //@{-----------------------------------------------------------------
   
   inline       Point<N, T> &operator =(const Point<N, T> &v);
   inline       Point<N, T> &operator+=(const Vector<N - 1, T> &rhs);
   inline       Point<N, T> &operator-=(const Vector<N - 1, T> &rhs);
   
   
   //@}-----------------------------------------------------------------
   ///@name Scalar Mutator Operators
   //@{-----------------------------------------------------------------
   
   inline       Point<N, T> &operator*=(const T &scale);
   inline       Point<N, T> &operator/=(const T &scale);
   
   
   //@}-----------------------------------------------------------------
   ///@name Arithmetic Operators
   //@{-----------------------------------------------------------------
   
   inline       Point<N, T>  operator+ (const Vector<N - 1, T> &rhs) const;
   inline       Point<N, T>  operator- (const Vector<N - 1, T> &rhs) const;
   inline       Vector<N - 1, T> operator- (const Point<N, T> &rhs)  const;
   
   
   //@}-----------------------------------------------------------------
   ///@name Scalar Arithmetic Operators
   //@{-----------------------------------------------------------------
   
   inline       Point<N, T>  operator* (const T &scale) const;
   inline       Point<N, T>  operator/ (const T &scale) const;
   
   
   //@}-----------------------------------------------------------------
   ///@name More Complex Functionality
   //@{-----------------------------------------------------------------
   
   /// @returns whether or not this Point is the origin
   inline bool isZero() const;
   
   /// @returns the magnitude of the vector connecting this point to the one 
   ///    passed in
   inline T getDistance(const Point<N, T> &v) const; 
   
   /// @returns the squared magnitude of the vector connecting this point to 
   ///    the one passed in
   inline T getDistance2(const Point<N, T> &v) const;
   
   /// @returns the sum of the components in this Point
   inline T getSum() const;
   
   /// @returns the average of the components in this Point
   inline T getAverage() const;
   
   /// Cleans up point (0's out entries that are less than epsilon)
   void cleanup();
   
   
   //@}-----------------------------------------------------------------
   ///@name Specialized Point Functionality
   //@{-----------------------------------------------------------------
   
   /// Convenience Constructor (note w = 1 by default)
   inline explicit Point(const T &v0, const T &v1 = 0, const T &v2 = 0, 
                         const T &v3 = 1);
   
   
   //@}-----------------------------------------------------------------
};


/// @brief Homogeneous point in 3-space (contains a 4th "w" coordinate)
///    @note Point3 contains 4 real_ts
typedef Point<4> Point3;

/// @brief Homogeneous point in 2-space (contains a 3rd "w" coordinate)
///    @note Point2 contains 3 real_ts
typedef Point<3> Point2;


///@name Extra operators where Point is on right-hand side
//@{---------------------------------------------------------------------

/// @returns the N-length vector resulting from multiplying a scalar by an 
///    N-length point
template <unsigned N, typename T>
MILTON_DLL_EXPORT
   inline Point<N, T> operator* (const T &scale, const Point<N, T> &rhs);

/// @returns (-1) * rhs, which is a negated version of the original right 
///    hand side point
template <unsigned N, typename T>
MILTON_DLL_EXPORT
   inline Point<N, T> operator- (const Point<N, T> &rhs);

/// Prints a Point to an output stream
template <unsigned N, typename T>
MILTON_DLL_EXPORT
   inline std::ostream     &operator<<(std::ostream &os, const Point<N, T> &v);

//@}---------------------------------------------------------------------


}

/* Include inline implementations */
#include "common/math/Point.inl"

namespace milton {

///@name Routines with point-specific functionality
//@{---------------------------------------------------------------------------

/// @brief Homogenizes the point passed in such that the 4th value ("w") is one
extern MILTON_DLL_EXPORT void homogenize(Point3 &p);

//@}---------------------------------------------------------------------------

}

#endif // POINT_H_

