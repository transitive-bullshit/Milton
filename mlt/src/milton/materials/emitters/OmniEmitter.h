/**<!-------------------------------------------------------------------->
   @class  OmniEmitter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Point emitter / light which emits light uniformly in every direction, 
   defined at a single point in 3-space (omnidirectional)
   <!-------------------------------------------------------------------->**/

#ifndef OMNI_EMITTER_H_
#define OMNI_EMITTER_H_

#include <materials/Emitter.h>
#include <stats/samplers/UniformOnSphereSampler.h>

namespace milton {

class MILTON_DLL_EXPORT OmniEmitter : public Emitter {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   OmniEmitter(SurfacePoint &pt, Material *parent = NULL)
         : Emitter(pt, new UniformOnSphereSampler(3), parent)
      { }
      
      virtual ~OmniEmitter()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual SpectralSampleSet getLe(const Vector3 &wo);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // OMNI_EMITTER_H_

