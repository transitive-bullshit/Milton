/**<!-------------------------------------------------------------------->
   @class  AABB
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      3-Dimensional Axis-Aligned Bounding Box
   <!-------------------------------------------------------------------->**/

#ifndef AABB_H_
#define AABB_H_

#include <common/math/algebra.h>
#include <ostream>

namespace milton {

struct Ray;

struct MILTON_DLL_EXPORT AABB : public SSEAligned {
   Vector3 min;
   Vector3 max;
   
   ///@name Constructors
   //@{-----------------------------------------------------------------

   inline AABB()
      : min(Vector3::infinity()), max(Vector3::negativeInfinity())
   { }
   
   inline AABB(const Vector3 &min_, const Vector3 &max_)
      : min(min_), max(max_)
   { }
   
   
   //@}-----------------------------------------------------------------
   ///@name Main usage interface
   //@{-----------------------------------------------------------------
   
   inline bool intersects(const Ray &ray, real_t &tMin, real_t &tMax) const;
   
   /// unions this AABB with the given vector
   inline void add(const Vector3 &v);
   
   /// unions this AABB with the given Point
   inline void add(const Point3  &p);
   
   /// unions this AABB with the given AABB
   inline void add(const AABB &aabb);
   
   /// @returns whether or not this AABB intersects the given AABB
   /// @note inclusive with respect to boundaries
   inline bool intersects(const AABB &aabb) const;
   
   /// @returns whether or not this AABB contains the given point
   /// @note inclusive with respect to boundaries of AABB
   inline bool contains(const Point3 &p) const;
   
   void preview() const;
   
   //@}-----------------------------------------------------------------
   ///@name Accessors
   //@{-----------------------------------------------------------------
   
   /// @returns the diagonal of this AABB
   inline Vector3 getDiagonal() const;
   
   inline Point3  getCenter() const;
   
   /// @returns the dominant (longest) axis along this AABB
   inline unsigned getMaxExtent() const;
   /// @returns the shortest axis along this AABB
   inline unsigned getMinExtent() const;
   
   /// @returns the surface area of this AABB
   inline real_t   getSurfaceArea() const;
   
   /// @returns the surface area of the child AABBs split along the specified 
   ///    @axis at @pos
   /// @note this is used extensively by the SAH kd-Tree
   inline void     getMinMaxSurfaceArea(const unsigned axis, 
                                        const real_t pos, 
                                        real_t &leftArea, 
                                        real_t &rightArea) const;
   
   /// @returns the tightest-fitting AABB after undergoing a transformation
   inline AABB     getTransformed(const Matrix4x4 &trans) const;
   
   /// @returns true if this AABB has been initialized with a min <= max
   /// @note will be valid if 'min' is less than or equal to 'max'
   inline bool     isValid() const;
   
   /// @returns true if this AABB represents a single point (min == max)
   /// @note will be valid if 'min' is strictly less than 'max'
   inline bool     isPoint() const;
   
   //@}-----------------------------------------------------------------
};

/// Prints an AABB to an output stream
MILTON_DLL_EXPORT inline std::ostream &operator<<(std::ostream &os, const AABB &aabb);

}

/* Include inline implementations */
#include "accel/AABB.inl"

#endif // AABB_H_

