/**<!-------------------------------------------------------------------->
   @class  DiffuseBSDF
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Ideal diffuse BSDF (lambertian) defined at a single point on a 
   surface in 3-space
   
   @param
      kd - SpectralSampleSet which scales / attenuates the overall reflectance 
   of the material (diffuse albedo)
   <!-------------------------------------------------------------------->**/

#ifndef DIFFUSE_BSDF_H_
#define DIFFUSE_BSDF_H_

#include <materials/BSDF.h>

namespace milton {

class MILTON_DLL_EXPORT DiffuseBSDF : public BSDF {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit DiffuseBSDF(SurfacePoint &pt, Material *parent = NULL)
         : BSDF(pt, parent)
      { }
      
      virtual ~DiffuseBSDF()
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

#endif // DIFFUSE_BSDF_H_

