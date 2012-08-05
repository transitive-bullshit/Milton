/**<!-------------------------------------------------------------------->
   @class  NullEmitter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Null emitter which doesn't emit any light, defined at a single point 
   in 3-space
   <!-------------------------------------------------------------------->**/

#ifndef NULL_EMITTER_H_
#define NULL_EMITTER_H_

#include <materials/Emitter.h>

namespace milton {

class MILTON_DLL_EXPORT NullEmitter : public Emitter {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit NullEmitter(SurfacePoint &pt, Material *parent = NULL)
         : Emitter(pt, NULL, parent)
      { }
      
      virtual ~NullEmitter()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization that may be necessary before beginning 
       * to use this Emitter
       * 
       * @note initializes spectral power and radiant exitance to black
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the spectral radiance emitted along the given vector
       * 
       * @note returns SpectralSampleSet::black (null emitter)
       */
      virtual SpectralSampleSet getLe(const Vector3 &wo);
      
      /**
       * @brief
       *    Sets up OpenGL material state (light properties) to enable 
       * a crude/fast preview of this light in OpenGL
       *
       * @note overrides default implementation to not modify GL light state
       *    at all
       */
      virtual void preview(Shape *shape);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // NULL_EMITTER_H_

