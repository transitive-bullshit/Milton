/**<!-------------------------------------------------------------------->
   @class  Cylinder
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a vertical 3D cylinder enclosed in the unit box 
   (-.5,-.5,-.5) to (.5,.5,.5)
   <!-------------------------------------------------------------------->**/

#ifndef CYLINDER_H_
#define CYLINDER_H_

#include <shapes/Implicit.h>

namespace milton {

class MILTON_DLL_EXPORT Cylinder : public Implicit {
   public:
      inline Cylinder() 
         : m_quadric(NULL)
      { }
      
      virtual ~Cylinder() {
         if (m_quadric) {
            gluDeleteQuadric(m_quadric);
            m_quadric = NULL;
         }
      }
      
      
      ///@name Intersection routines
      //@{-----------------------------------------------------------------
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual bool intersects(const Ray &ray, real_t tMax = INFINITY);
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      virtual void preview();
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
      
   protected:
      GLUquadric *m_quadric;
};

}

#endif // CYLINDER_H_

