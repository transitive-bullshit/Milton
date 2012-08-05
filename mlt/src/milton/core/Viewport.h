/**<!-------------------------------------------------------------------->
   @class  Viewport
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a square domain over [0,1]^2, overlaid with a conceptual grid 
   of uniformly-sized tiles (representing pixels)
   <!-------------------------------------------------------------------->**/

#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#include <utils/PropertyMap.h>
#include <common/math/Point.h>

namespace milton {

struct MILTON_DLL_EXPORT Viewport {
   
   ///@name Constructors
   //@{-----------------------------------------------------------------
   
   inline Viewport(unsigned width, unsigned height)
      : m_width(width), m_height(height), 
        m_invWidth(1.0 / width), m_invHeight(1.0 / height)
   { }
   
   inline Viewport(unsigned noSamples) {
      const unsigned side = (unsigned) sqrt((real_t) noSamples);
      
      m_width  = side;
      m_height = side;
      
      m_invWidth  = 1.0 / side;
      m_invHeight = m_invWidth;
   }
   
   inline Viewport()
      : m_width(1), m_height(1), m_invWidth(1), m_invHeight(1)
   { }
   
   
   //@}-----------------------------------------------------------------
   ///@name Accessors
   //@{-----------------------------------------------------------------
   
   /// @returns the number of bins within one row of this domain
   inline unsigned getWidth() const {
      return m_width;
   }
   
   /// @returns the number of bins within one column of this domain
   inline unsigned getHeight() const {
      return m_height;
   }
   
   /// @returns the horizontal length of a single bin in this domain
   inline real_t getInvWidth() const {
      return m_invWidth;
   }
   
   /// @returns the vertical length of a single bin in this domain
   inline real_t getInvHeight() const {
      return m_invHeight;
   }
   
   /// @returns the total number of bins in this domain (width * height)
   inline unsigned getSize() const {
      return m_width * m_height;
   }
   
   /**
    * @returns the integer bin along this domain which contains the given @p pt
    *    lying in the unit interval after being mapped to an image of the width 
    *    and height of this domain
    */
   inline void getBin(const Point2 &pt, unsigned &outX, unsigned &outY) const {
      outX = (unsigned) CLAMP(floor(pt[0] * m_width ), 0, m_width  - 1);
      outY = (unsigned) CLAMP(floor(pt[1] * m_height), 0, m_height - 1);
   }
   
   
   //@}-----------------------------------------------------------------
   ///@name Mutators
   //@{-----------------------------------------------------------------
   
   /// Sets the number of rows and columns in this domain
   inline void setSize(unsigned width, unsigned height) {
      setWidth(width);
      setHeight(height);
   }
   
   /// Sets the number of bins in one row of this domain
   inline void setWidth(unsigned width) {
      m_width    = width;
      m_invWidth = 1.0 / width;
   }
   
   /// Sets the number of bins in one column of this domain
   inline void setHeight(unsigned height) {
      m_height    = height;
      m_invHeight = 1.0 / height;
   }
   
   
   //@}-----------------------------------------------------------------
   
   private:
      unsigned m_width;
      unsigned m_height;
      
      real_t   m_invWidth;
      real_t   m_invHeight;
};

}

#endif // VIEWPORT_H_
