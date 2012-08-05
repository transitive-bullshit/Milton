/**<!-------------------------------------------------------------------->
   @class  InstancedShape
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Acts as a proxy for a shape which has been instanced
   <!-------------------------------------------------------------------->**/

#ifndef INSTANCED_SHAPE_H_
#define INSTANCED_SHAPE_H_

#include <shapes/Transformable.h>

namespace milton {

class MILTON_DLL_EXPORT InstancedShape : public Transformable {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline InstancedShape(Shape *instancee = NULL)
         : Transformable(), m_instancee(instancee)
      { }
      
      virtual ~InstancedShape()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes this InstancedShape, assuming the underlying instancee 
       * has already been initialized
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Tests the given ray with this object for intersection. If a valid 
       * intersection exists, its "t" value will be returned and the given 
       * SurfacePoint will be initialized with enough data for this object to 
       * later fill in all relevant information lazily (world space normal 
       * at intersection point, uv coordinates, etc.)
       * 
       * @note if you only care about whether or not an intersection exists 
       *    and don't actually need to know the exact intersection point, 
       *    see the intersects method instead.
       * @returns the smallest positive "t" value of any intersections 
       *    found, or INFINITY if no valid intersection exists
       */
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      /**
       * @brief
       *    Used for occlusion/visibility testing where you don't necessarily 
       * care about anything other than whether or not an intersection exists.
       * (generally faster than getIntersection)
       * 
       * @note default implementation defers to getIntersection
       * @returns whether or not the given ray intersects this object with a 
       *    positive "t" value greater than EPSILON and less than the given 
       *    @p tMax
       */
      virtual bool intersects(const Ray &ray, real_t tMax = INFINITY);
      
      /**
       * @returns whether or not this Shape supports the concept of a geometric
       *    surface normal defined over its surface
       * 
       * @note default implementation returns true
       */
      virtual bool hasNormal() const;
      
      /**
       * @brief
       *    Displays a crude OpenGL preview of this shape
       * 
       * @note default implementation calls preview on this shape's material
       */
      virtual void preview();
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a point chosen uniformly random over the surface of this 
       *    shape in 'pt'
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
      ///@name Lazy evaluation of SurfacePoint information
      //@{-----------------------------------------------------------------
      
      virtual void initSurfacePoint(SurfacePoint &pt) const;
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
      
      virtual real_t _getSurfaceArea();
      
      //@}-----------------------------------------------------------------
      
   protected:
      Shape *m_instancee;
};

}

#endif // INSTANCED_SHAPE_H_

