/**<!-------------------------------------------------------------------->
   @file   Transformable.inl
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a Shape which is defined in its own local "object space" and
   knows how to transform itself into the global "world space"
   <!-------------------------------------------------------------------->**/

#ifndef TRANSFORMABLE_INL_
#define TRANSFORMABLE_INL_

#include <core/Ray.h>

namespace milton {

inline void Transformable::_transformRayWorldToObj(const Ray &ray, Point3 &p, 
                                                   Vector3 &d) const
{
   p = m_transToWorldInv * ray.origin;
   d = m_transToWorldInv * ray.direction;
}

inline void Transformable::_transformPoint3WorldToObj(const Point3 &pWorld, 
                                                      Point3 &pObj) const
{
   pObj = m_transToWorldInv * pWorld;
}

inline void Transformable::_transformVector3WorldToObj(const Vector3 &vWorld, 
                                                       Vector3 &vObj) const
{
   vObj = m_transToWorldInv * vWorld;
}
      
inline void Transformable::_transformVector3ObjToWorld(const Vector3 &nObj, 
                                                       Vector3 &nWorld) const
{
   const real_t *m = *m_transToWorldInv;
   
   // transpose upper 3x3 of m_transToWorldInv and multiply by nObj all at once
   nWorld = Vector3(nObj[0] * m[0] + nObj[1] * m[4] + nObj[2] * m[8], 
                    nObj[0] * m[1] + nObj[1] * m[5] + nObj[2] * m[9], 
                    nObj[0] * m[2] + nObj[1] * m[6] + nObj[2] * m[10]);
   
   nWorld.normalize();
}

}

#endif // TRANSFORMABLE_INL_

