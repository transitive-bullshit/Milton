/**<!-------------------------------------------------------------------->
   @class  MetaBall
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

#ifndef META_BALL_H_
#define META_BALL_H_

#include <shapes/MetaObject.h>

namespace milton {

class MILTON_DLL_EXPORT MetaBall : public MetaObject {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MetaBall(real_t strength = 1.0, bool negative = false, 
                      const Point3 &position = Point3(), 
                      real_t radius = 1.0)
         : MetaObject(strength, negative), 
           m_position(position), m_radius(radius)
      { }
      
      virtual ~MetaBall()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization which may be necessary before 
       * calling getAABB or evaluate
       */
      virtual void init();
      
      /**
       * @returns the bounding box of influence which bounds the range in 
       *    which this MetaBall may have a non-zero charge
       */
      virtual AABB getAABB() const;
      
      /**
       * @returns this MetaBall's charge at the given point
       * 
       * @note
       *    If the given point is outside of this MetaBall's AABB, 
       * evaluate is assumed to return zero
       */
      virtual real_t evaluate(const Point3 &pt) const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      /// @returns the center of this MetaBall
      inline Point3 getPosition() const {
         return m_position;
      }
      
      /// sets the initial charge of this object
      inline void setPosition(const Point3 &position) {
         m_position = position;
      }
      
      /// @returns the radius of this MetaBall
      inline real_t getRadius() const {
         return m_radius;
      }
      
      /// sets the radius of this MetaBall
      inline void setRadius(real_t radius) {
         m_radius = radius;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      AABB   m_aabb;
      Point3 m_position;
      real_t m_radius;
};

}

#endif // META_BALL_H_

