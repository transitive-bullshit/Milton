/**<!-------------------------------------------------------------------->
   @class  Sphere
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of a 3D sphere with radius .5, enclosed in the unit box 
   (-.5,-.5,-.5) to (.5,.5,.5)
   <!-------------------------------------------------------------------->**/

#ifndef SPHERE_H_
#define SPHERE_H_

#include <shapes/Implicit.h>
#include <stats/samplers/UniformOnSphereSampler.h>

namespace milton {

class MILTON_DLL_EXPORT Sphere : public Implicit {
   public:
      inline Sphere(Material *material = NULL) 
         : Implicit(material), 
           m_quadric(NULL), m_surfaceSampler(NULL)
      { }
      
      virtual ~Sphere() {
         if (m_quadric) {
            gluDeleteQuadric(m_quadric);
            m_quadric = NULL;
         }
         
         safeDelete(m_surfaceSampler);
      }
      
      
      ///@name Intersection routines
      //@{-----------------------------------------------------------------
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual bool intersects(const Ray &ray, 
                              real_t tMax = INFINITY);
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      virtual void preview();
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a point chosen uniformly random over the surface of this 
       *    shape
       * @note the probability density of selecting this point is assumed to 
       *    be (1.0 / getSurfaceArea())
       */
      virtual void getRandomPoint(SurfacePoint &pt);
      
      /**
       * @returns the point on the surface of this shape corresponding to the 
       *    given UV coordinates
       */
      virtual Point3 getPosition(const UV &uv);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
      
      /**
       * @returns the total surface area of this shape
       * 
       * @TODO return surface area in world-space!!!
       */
      virtual real_t _getSurfaceArea();
      
   protected:
      GLUquadric             *m_quadric;
      UniformOnSphereSampler *m_surfaceSampler;
};

}

#endif // SPHERE_H_

