/**<!-------------------------------------------------------------------->
   @class  Blob
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      Blobby isosurface composed of a set of MetaObjects, all of which 
   combine to form a scalar field whose contour at this blob's threshold 
   value defines the surface. Upon initialization (Blob::init), the implicit 
   surface is polygonized into triangles using a variant of the well-known 
   Marching Cubes algorithm.
      A MetaObject exerts a positive or negative 'charge' in a scalar field, 
   whose isocontours (level sets) define blobby surfaces with different 
   threshold values corresponding to the contour level.  MetaObjects positively
   or negatively affect their neighboring MetaObjects depending on their 
   'strength' and 'negative' attributes.
   
   @param
      threshold - specifies the particular isocontour boundary which will 
   define the surface of this blobby shape (defaults to BLOB_THRESHOLD_DEFAULT)
   
   @see also MetaObject
   <!-------------------------------------------------------------------->**/

#ifndef BLOB_H_
#define BLOB_H_

#include <shapes/MetaObject.h>
#include <shapes/Transformable.h>

#define BLOB_THRESHOLD_DEFAULT (0.17) 

namespace milton {

DECLARE_STL_TYPEDEF(std::vector<MetaObject*>, MetaObjectList);

class Mesh;

class MILTON_DLL_EXPORT Blob : public MetaObject, public Transformable {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline Blob(real_t threshold = BLOB_THRESHOLD_DEFAULT)
         : MetaObject(), Transformable(), 
           m_mesh(NULL), m_threshold(threshold)
      { }
      
      inline Blob(const MetaObjectList &metaObjects, 
                  real_t threshold = BLOB_THRESHOLD_DEFAULT)
         : MetaObject(), Transformable(), 
           m_metaObjects(metaObjects), 
           m_mesh(NULL), m_threshold(threshold)
      { }
      
      virtual ~Blob();
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      virtual void init();
      virtual void preview();
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual bool   intersects(const Ray &ray, real_t tMax = INFINITY);
      
      /**
       * @returns the aggregate charge at the given point
       * 
       * @note
       *    If the given point is outside of this Blob's AABB, evaluate is 
       * assumed to return zero
       */
      virtual real_t evaluate(const Point3 &pt) const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the threshold spcifying the boundary of this blobby surface
       */
      inline real_t getThreshold() const {
         return m_threshold;
      }
      
      /**
       * @brief
       *    sets the threshold spcifying the boundary of this blobby surface
       */
      inline void setThreshold(real_t threshold) {
         m_threshold = threshold;
      }
      
      /**
       * @returns the underlying mesh generated from marching cubes
       */
      inline Mesh *getMesh() {
         return m_mesh;
      }
      
      /**
       * @brief
       *    adds the given MetaObject to this blob
       */
      inline void push_back(MetaObject *meta) {
         ASSERT(meta);
         
         m_metaObjects.push_back(meta);
      }
      
      inline MetaObjectList &getMetaObjects() {
         return m_metaObjects;
      }
      
      virtual AABB getAABB() const;
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
      
   protected:
      MetaObjectList m_metaObjects;
      Mesh          *m_mesh;
      
      real_t         m_threshold;
};

}

#endif // BLOB_H_

