/**<!-------------------------------------------------------------------->
   @class  Implicit
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract representation of an implicit shape in 3-space.
   <!-------------------------------------------------------------------->**/

#ifndef IMPLICIT_H_
#define IMPLICIT_H_

#include <shapes/Transformable.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define INTERSECT_TOLERANCE    (EPSILON)
#define isValidT(t, min)       ((t) < (min) && (t) > INTERSECT_TOLERANCE)

namespace milton {

class MILTON_DLL_EXPORT Implicit : public Transformable {
   public:
      inline Implicit(Material *material = NULL) 
         : Transformable(material)
      { }
      
      virtual ~Implicit()
      { }
      
      /**
       * @brief
       *    Initializes this Implicit's object-space AABB
       * 
       * @note defaults to unit box from (-.5,-.5,-.5) to (.5,.5,.5)
       */
      virtual void init();
};

}

#endif // IMPLICIT_H_

