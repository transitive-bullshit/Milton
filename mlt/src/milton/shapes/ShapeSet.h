/**<!-------------------------------------------------------------------->
   @class  ShapeSet
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a collection of shapes which all have the same transformation
   <!-------------------------------------------------------------------->**/

#ifndef SHAPE_SET_H_
#define SHAPE_SET_H_

#include <shapes/Transformable.h>
#include <utils/PropertyMap.h>

namespace milton {

class SpatialAccel;

class MILTON_DLL_EXPORT ShapeSet : public Transformable, public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline ShapeSet(bool ownMemory = true)
         : Transformable(), PropertyMap(), 
           m_primitives(), m_spatialAccel(NULL), 
           m_ownMemory(ownMemory)
      { }
      
      inline ShapeSet(const PrimitiveList &primitiveList, 
                      bool ownMemory = true)
         : Transformable(), PropertyMap(), 
           m_primitives(primitiveList), m_spatialAccel(NULL), 
           m_ownMemory(ownMemory)
      { }
      
      virtual ~ShapeSet();
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      virtual void init(bool initChildren);
      virtual void init();
      
      virtual void preview();
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual bool   intersects(const Ray &ray, real_t tMax = INFINITY);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline unsigned size() const {
         return m_primitives.size();
      }
      
      inline Shape *operator[](unsigned int index) {
         ASSERT(index >= 0 && index < size());
         
         return m_primitives[index];
      }
      
      inline void push_back(Shape *shape) {
         ASSERT(shape);
         
         m_primitives.push_back(shape);
      }
      
      inline void clear() {
         m_primitives.clear();
      }
      
      inline PrimitiveList &getPrimitives() {
         return m_primitives;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
      
      /**
       * @returns the total surface area of this shape
       */
      virtual real_t _getSurfaceArea();
      
   protected:
      PrimitiveList     m_primitives;
      SpatialAccel     *m_spatialAccel;
      bool              m_ownMemory;
};

}

#endif // SHAPE_SET_H_

