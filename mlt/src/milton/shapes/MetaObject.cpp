/**<!-------------------------------------------------------------------->
   @file   MetaObject.cpp
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

#include "MetaObject.h"

namespace milton {

void MetaObject::init() {
   m_strength   = getValue<real_t>("strength", m_strength);
   m_isNegative = getValue<bool>  ("negative", m_isNegative);
}

}

