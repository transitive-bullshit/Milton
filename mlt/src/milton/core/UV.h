/**<!-------------------------------------------------------------------->
   @class  UV
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      UV coordinate wrapper for texture coordinates
   
   @note u and v should lie in [0, 1]
   <!-------------------------------------------------------------------->**/

#ifndef UV_H_
#define UV_H_

#include <common/math/algebra.h>

namespace milton {

struct MILTON_DLL_EXPORT UV {
   union {
      struct {
         real_t u;
         real_t v;
      };
      
      real_t data[2];
   };
   
   inline UV(real_t u_, real_t v_)
      : u(u_), v(v_)
   { }
   
   inline UV(const Point2 &p)
      : u(p[0]), v(p[1])
   { }
   
   inline UV() 
      : u(0), v(0)
   { }
   
   inline real_t operator[](unsigned index) const {
      ASSERT(index >= 0 && index <= 1);
      
      return data[index];
   }
   
   inline real_t &operator[](unsigned index) {
      ASSERT(index >= 0 && index <= 1);
      
      return data[index];
   }
};

}

#endif // UV_H_
