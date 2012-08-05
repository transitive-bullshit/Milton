/**<!-------------------------------------------------------------------->
   @class  Cone
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a vertical 3D cone enclosed in the unit box 
   (-.5,-.5,-.5) to (.5,.5,.5) -- the tip is at (0,.5,0)
   <!-------------------------------------------------------------------->**/

#ifndef CONE_H_
#define CONE_H_

#include <shapes/Implicit.h>

namespace milton {

class MILTON_DLL_EXPORT Cone : public Implicit {
   public:
      inline Cone() 
         : m_quadric(NULL)
      { }
      
      virtual ~Cone() {
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

#endif // CONE_H_

