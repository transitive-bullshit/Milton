/**<!-------------------------------------------------------------------->
   @class  AbsorbentBSDF
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Ideal, cool, black-body absorbent material (absorbs all incoming light)
   <!-------------------------------------------------------------------->**/

#ifndef ABSORBENT_BSDF_H_
#define ABSORBENT_BSDF_H_

#include <materials/BSDF.h>

namespace milton {

class MILTON_DLL_EXPORT AbsorbentBSDF : public BSDF {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit AbsorbentBSDF(SurfacePoint &pt, Material *parent = NULL)
         : BSDF(pt, parent)
      { }
      
      virtual ~AbsorbentBSDF()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual Event sample (const Event &event);
      
      virtual real_t getPd(const Event &event);
      
      virtual SpectralSampleSet evaluate(const Vector3 &wi, const Vector3 &wo);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // ABSORBENT_BSDF_H_

