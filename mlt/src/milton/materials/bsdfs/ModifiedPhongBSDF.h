/**<!-------------------------------------------------------------------->
   @class  ModifiedPhongBSDF
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Physically-correct modified phong model for glossy/specular surfaces, 
   defined at a single point on a surface in 3-space. The modified phong 
   model has two inputs: Kd, and Ks in [0, 1] subject to Kd + Ks <= 1, 
   where Kd and Ks represent the diffuse and specular reflectivity of the 
   surface respectively (fraction of incoming energy that is reflected 
   diffusely/specularly). A third input, n, represents the specular 
   shininess of the surface, where higher values of n correspond to tighter /
   sharper specular highlights, and lower values of n correspond to wider / 
   glossier highlights.
   
   @note For more information, please see: 
      E. Lafortune and Y. Willems. Using the modified Phong reflectance 
   model for physically based rendering. Technical Report CW197, Dept of 
   Computer Science, K.U. Leuven, 1994.
   
   @param
      kd - SpectralSampleSet which scales / attenuates the overall reflectance 
   of the material (diffuse albedo)
   
   @param
      ks - SpectralSampleSet which scales / attenuates the overall reflectance 
   of the material (specular albedo)
   
   @param
      n  - Wavelength-dependent shininess exponent
   
   @note Also known as modified Blinn-Phong model
   <!-------------------------------------------------------------------->**/

#ifndef MODIFIED_PHONG_BSDF_H_
#define MODIFIED_PHONG_BSDF_H_

#include <materials/BSDF.h>

namespace milton {

class MILTON_DLL_EXPORT ModifiedPhongBSDF : public BSDF {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit ModifiedPhongBSDF(SurfacePoint &pt, 
                                        Material *parent = NULL)
         : BSDF(pt, parent), m_n(SpectralSampleSet::fill(-1))
      { }
      
      virtual ~ModifiedPhongBSDF()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual Event sample (const Event &);
       
      virtual real_t getPd(const Event &event);
      
      virtual SpectralSampleSet evaluate(const Vector3 &wi, const Vector3 &wo);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      SpectralSampleSet m_kd;
      SpectralSampleSet m_ks;
      SpectralSampleSet m_n;
      
      real_t   m_kda; // average kd
      real_t   m_ksa; // average ks
      real_t   m_na;  // average n
};

}

#endif // MODIFIED_PHONG_BSDF_H_

