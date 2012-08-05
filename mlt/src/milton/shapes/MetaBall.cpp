/**<!-------------------------------------------------------------------->
   @file   MetaBall.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      A MetaObject exerts a positive or negative 'charge' in a scalar field, 
   whose isocontours (level sets) define blobby surfaces with different 
   threshold values corresponding to the contour level.  MetaObjects positively
   or negatively affect their neighboring MetaObjects depending on their 
   'strength' and 'negative' attributes.
      A MetaBall exerts a non-zero charge (aka influence) over a spherical 
   region emanating from a single point, 'position', out to a maximum distance 
   of 'radius'.
   
   @param
      position - center of sphere defining region of influence
   @param
      radius   - maximum extent of influence
   
   @see also Blob
   @see also MetaObject
   <!-------------------------------------------------------------------->**/

#include "MetaBall.h"

#define MAX_UNIT_DISTANCE           (create_real(0.707))
#define MAX_UNIT_DISTANCE_SQUARED   (create_real(0.5))

namespace milton {

void MetaBall::init() {
   MetaObject::init();
   
   const Vector3 &position = getValue<Vector3>("position", Vector3(m_position.data));
   
   m_position = Point3(position[0], position[1], position[2]);
   m_radius   = getValue<real_t> ("radius", m_radius);
   
   ASSERT(m_radius >= 0);
   
   const real_t maxDist  = MAX_UNIT_DISTANCE * m_radius;
   const Vector3 &offset = Vector3(maxDist, maxDist, maxDist);
   
   m_aabb = AABB(position - offset, position + offset);
   //cerr << "MetaBall: " << m_aabb << endl;
   //cerr << "\t" << "radius: " << m_radius << endl;
}

AABB MetaBall::getAABB() const {
   return m_aabb;
}

real_t MetaBall::evaluate(const Point3 &pt) const {
   real_t distSquared = (pt - m_position).getMagnitude2();
   distSquared       /= m_radius * m_radius;
   
   // same as if sqrt(distSquared) < MAX_UNIT_DISTANCE
   if (distSquared < MAX_UNIT_DISTANCE_SQUARED) {
      // charge(x)    = m_strength * intensity(||x - origin||)
      // intensity(d) = d^4 - d^2 + 0.25
      // 
      // note: (d^2 * (d^2 - 1)) = (d^2)^2 - (d^2) = d^4 - d^2
      
      return m_strength * (distSquared * (distSquared - 1) + 0.25);
   }
   
   return 0;
}

}

