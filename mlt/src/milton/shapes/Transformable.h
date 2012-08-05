/**<!-------------------------------------------------------------------->
   @class  Transformable
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a Shape which is defined in its own local "object space" and
   knows how to transform itself into the global "world space"
   <!-------------------------------------------------------------------->**/

#ifndef TRANSFORMABLE_H_
#define TRANSFORMABLE_H_

#include <shapes/Shape.h>
#include <common/math/algebra.h>

namespace milton {

class MILTON_DLL_EXPORT Transformable : public Shape {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline Transformable(const Matrix4x4 &transToWorld, 
                           const Matrix4x4 &transToWorldInv, 
                           Material *material = NULL)
         : Shape(material), 
           m_transToWorld(transToWorld), 
           m_transToWorldInv(transToWorldInv)
      { }
      
      inline Transformable(const Matrix4x4 &transToWorld, 
                           Material *material = NULL)
         : Shape(material), 
           m_transToWorld(transToWorld), 
           m_transToWorldInv(transToWorld.getInverse())
      { }
      
      inline Transformable(Material *material = NULL)
         : Shape(material), 
           m_transToWorld(Matrix4x4::identity()), 
           m_transToWorldInv(Matrix4x4::identity())
      { }
      
      virtual ~Transformable()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes this Transformable's world-space AABB, assuming a 
       * derived class has already filled in m_objSpaceAABB
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * Display a crude OpenGL preview of this shape
       * 
       * @note
       *    Default implementation sets the ModelView matrix appropriately
       * with respect to this Transformable's transformation
       */
      virtual void preview();
      
       
      //@}-----------------------------------------------------------------
      ///@name Sampling functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @note implementation returns the origin of this shape in world space 
       *    because not all shapes support this functionality
       */
      virtual Point3 getPosition(const UV &uv);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline const Matrix4x4 &getTransToWorld() const {
         return m_transToWorld;
      }
      
      inline void setTransToWorld(const Matrix4x4 &transToWorld) {
         m_transToWorld = transToWorld;
         m_transToWorldInv = transToWorld.getInverse();
      }
      
      inline const Matrix4x4 &getTransToWorldInv() const {
         return m_transToWorldInv;
      }
      
      inline void setTransToWorldInv(const Matrix4x4 &transToWorldInv) {
         m_transToWorldInv = transToWorldInv;
      }
      
      inline const AABB &getObjSpaceAABB() const {
         return m_objSpaceAABB;
      }
      
      /**
       * @returns true iff this Shape subclasses Transformable
       */
      virtual bool isTransformable() const {
         return true;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      void _transformRayWorldToObj(const Ray &ray, Point3 &p, 
                                   Vector3 &d) const;
      
      void _transformPoint3WorldToObj(const Point3 &pWorld, 
                                      Point3 &pObj) const;
      
      void _transformVector3WorldToObj(const Vector3 &vWorld, 
                                       Vector3 &vObj) const;
      
      void _transformVector3ObjToWorld(const Vector3 &nObj, 
                                       Vector3 &nWorld) const;
      
   protected:
      Matrix4x4 m_transToWorld;
      Matrix4x4 m_transToWorldInv;
      
      AABB      m_objSpaceAABB;
};

}

/* Include inline implementations */
#include <shapes/Transformable.inl>

#endif // TRANSFORMABLE_H_

