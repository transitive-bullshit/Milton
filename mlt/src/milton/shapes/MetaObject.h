/**<!-------------------------------------------------------------------->
   @class  MetaObject
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      A MetaObject exerts a positive or negative 'charge' in a scalar field, 
   whose isocontours (level sets) define blobby surfaces with different 
   threshold values corresponding to the contour level.  MetaObjects positively
   or negatively affect their neighboring MetaObjects depending on their 
   'strength' and 'negative' attributes.  'strength' defines the initial charge 
   of the object, and 'negative' is a boolean which defaults to false, denoting 
   whether or not this MetaObject has a positive or a negative impact on 
   surrounding MetaObjects.  MetaObject is an abstract class, and specific 
   implementations define how its initial charge dissipates throughout the 
   field.
   
   @param
      strength - initial charge of object (defaults to 1.0)
   @param
      negative - whether or not this metaobject has a positive or negative 
   impact on surrounding MetaObjects (defaults to false)
   
   @see also Blob
   @see also MetaBall
   <!-------------------------------------------------------------------->**/

#ifndef META_OBJECT_H_
#define META_OBJECT_H_

#include <shapes/ScalarField.h>
#include <utils/PropertyMap.h>
#include <accel/AABB.h>

namespace milton {

class MILTON_DLL_EXPORT MetaObject : public ScalarField, public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MetaObject(real_t strength = 1.0, bool negative = false)
         : ScalarField(), PropertyMap(), 
           m_strength(strength), m_isNegative(negative)
      { }
      
      virtual ~MetaObject()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization which may be necessary before 
       * calling getAABB or evaluate
       * 
       * @note
       *    Implementations will likely want to precompute their AABB in 
       * init and store it for later since getAABB may be called often
       * 
       * @note
       *    Default implementation synchs parameters from PropertyMap
       */
      virtual void init();
      
      /**
       * @returns the bounding box of influence which bounds the range in 
       *    which this MetaObject may have a non-zero charge
       */
      virtual AABB getAABB() const = 0;
      
      /**
       * @returns this MetaObject's charge at the given point
       * 
       * @note
       *    If the given point is outside of this MetaObject's AABB, 
       * evaluate is assumed to return zero
       */
      virtual real_t evaluate(const Point3 &pt) const = 0;
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      /// @returns the initial charge of this object
      inline real_t getStrength() const {
         return m_strength;
      }
      
      /// sets the initial charge of this object
      inline void setStrength(real_t strength) {
         m_strength = strength;
      }
      
      /// @returns whether or not this MetaObject has a positive or negative 
      ///    influence on surrounding MetaObjects
      inline bool   isNegative() const {
         return m_isNegative;
      }
      
      /// sets whether or not this MetaObject has a positive or negative 
      ///    influence on surrounding MetaObjects
      inline void setIsNegative(bool negative) {
         m_isNegative = negative;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      real_t m_strength;
      bool   m_isNegative;
};

}

#endif // META_OBJECT_H_

