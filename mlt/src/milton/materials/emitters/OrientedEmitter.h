/**<!-------------------------------------------------------------------->
   @class  OrientedEmitter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Area emitter / light which emits light uniformly across a 2D domain, 
   defined at a single point in 3-space, and restricted to the local positive 
   hemisphere (oriented along hemisphere about local surface normal)
   <!-------------------------------------------------------------------->**/

#ifndef ORIENTED_EMITTER_H_
#define ORIENTED_EMITTER_H_

#include <materials/Emitter.h>
#include <materials/bsdfs/DiffuseBSDF.h>

namespace milton {

class MILTON_DLL_EXPORT OrientedEmitter : public Emitter {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   OrientedEmitter(SurfacePoint &pt, Material *parent = NULL)
         : Emitter(pt, new DiffuseBSDF(pt), parent)
      { }
      
      virtual ~OrientedEmitter()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual SpectralSampleSet getLe(const Vector3 &wo);
      
      virtual void setWi(const Vector3 &wi);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // ORIENTED_EMITTER_H_

