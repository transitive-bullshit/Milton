/**<!-------------------------------------------------------------------->
   @class  DielectricBSDF
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Wavelength-dependent, thin dielectric BSDF defined at a single point on 
   a surface in 3-space. The opacity input controls the transmittance versus 
   reflectance of different wavelengths. A pure specular mirror may be obtained
   by creating a dielectric BSDF with zero opacity. Conversely, a purely 
   transparent surface may be obtained by creating a dielectric with full 
   opacity. Note that a completely transparent surface will still reflect some 
   light (in proportion to Fresnel's Laws) because refraction is undefined past 
   a critical angle when light is traveling between a lighter and denser 
   medium. When this happens, some light is reflected regardless of the opacity 
   parameter, and this is typically known as total internal reflection.
   
   @param
      opacity - SpectralSampleSet which specifies the transmittance of the 
   material
   
   @param
      ks      - SpectralSampleSet which scales / attenuates the overall 
   reflectance of the material (specular albedo)
   <!-------------------------------------------------------------------->**/

#ifndef DIELECTRIC_BSDF_H_
#define DIELECTRIC_BSDF_H_

#include <materials/BSDF.h>

namespace milton {

class MILTON_DLL_EXPORT DielectricBSDF : public BSDF {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit DielectricBSDF(SurfacePoint &pt, Material *parent = NULL)
         : BSDF(pt, parent)
      { }
      
      virtual ~DielectricBSDF()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual Event sample (const Event &event);
       
      virtual real_t getPd(const Event &event);
      
      virtual SpectralSampleSet evaluate(const Vector3 &wi, const Vector3 &wo);
      
      virtual bool isSpecular(Event &/* event unused*/) const {
         return true;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      SpectralSampleSet m_ior;
      SpectralSampleSet m_transparency;
      real_t            m_avgTransparency;
};

}

#endif // DIELECTRIC_BSDF_H_

