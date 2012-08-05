/**<!-------------------------------------------------------------------->
   @class  IndexOfRefraction
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Catalog of various common indices of refraction
   
   @see also Fresnel
   <!-------------------------------------------------------------------->**/

#ifndef INDEX_OF_REFRACTION_H_
#define INDEX_OF_REFRACTION_H_

#include <common/common.h>

namespace milton {

struct MILTON_DLL_EXPORT IndexOfRefraction {
   static const real_t AIR;
   static const real_t WATER;
   static const real_t ACRYLIC_GLASS;
   static const real_t DIAMOND;
};

}

#endif // INDEX_OF_REFRACTION_H_

