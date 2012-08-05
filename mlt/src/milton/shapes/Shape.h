/**<!-------------------------------------------------------------------->
   @class  Shape
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract representation of a shape in 3-space
   <!-------------------------------------------------------------------->**/

#ifndef SHAPE_H_
#define SHAPE_H_

#include <shapes/Intersectable.h>
#include <common/math/algebra.h>
#include <core/UV.h>

namespace milton {

class InstancedShape;
class Material;
class Shape;

DECLARE_STL_TYPEDEF(std::vector<Shape*>, PrimitiveList);

class MILTON_DLL_EXPORT Shape : public Intersectable, public SSEAligned {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   Shape(Material *material = NULL)
         : Intersectable(), m_material(material), m_surfaceArea(0)
      { }
      
      virtual ~Shape() 
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief 
       *    Initializes this Shape for fast intersection testing
       * 
       * @note default implementation ensures this shape has been given 
       *    a valid material
       * @note must be called before getIntersection or intersects
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
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
       * 
       * @note default implementation returns origin because not all shapes 
       *    support this functionality
       */
      virtual void getRandomPoint(SurfacePoint &pt);
      
      /**
       * @returns the point on the surface of this shape corresponding to the 
       *    given UV coordinates in 'pt'
       * 
       * @note default implementation returns origin because not all shapes 
       *    support this functionality
       */
      virtual void getPoint(SurfacePoint &pt, const UV &uv);
      
      /**
       * @returns the point on the surface of this shape corresponding to the 
       *    given UV coordinates
       * 
       * @note default implementation returns origin because not all shapes 
       *    support this functionality
       */
      virtual Point3 getPosition(const UV &uv);
      
      /**
       * @returns the total surface area of this shape
       * 
       * @note default implementation returns m_surfaceArea, which is assumed 
       *    to be calculated at shape initialization
       */
      virtual real_t getSurfaceArea();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      virtual AABB getAABB() const {
         return m_aabb;
      }
      
      inline void setMaterial(Material *material) {
         m_material = material;
      }
      
      inline Material *getMaterial() {
         return m_material;
      }
      
      /**
       * @returns true iff this Shape subclasses Transformable
       */
      virtual bool isTransformable() const {
         return false;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Lazy evaluation of SurfacePoint information
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes the given SurfacePoint which is assumed to lie on the 
       * surface of this Shape; fills in the point's UV coords, normal, and 
       * Material properties (@see Material::initSurfacePoint)
       */
      virtual void initSurfacePoint(SurfacePoint &pt) const;
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const = 0;
      virtual void _getGeometricNormal(SurfacePoint &pt) const = 0;
      
      /**
       * @returns the total surface area of this shape
       * 
       * @note default implementation returns 0 because not all shapes support
       *    this calculation (some are especially hard to compute in world 
       *    space)
       */
      virtual real_t _getSurfaceArea();
      
      friend class InstancedShape;
      
      //@}-----------------------------------------------------------------
      
   protected:
      /// World-space AABB
      AABB      m_aabb;
      
      /// Material describing reflectance properties over this shape's surface
      Material *m_material;
      
      /// World-space surface area of this shape
      real_t    m_surfaceArea;
};

}

#endif // SHAPE_H_

