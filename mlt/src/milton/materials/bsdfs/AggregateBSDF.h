/**<!-------------------------------------------------------------------->
   @class  AggregateBSDF
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Linear combination of different BSDF implementations, where the 
   coefficients are determined either a priori (inherent property described 
   in scenefile) or via a texture map lookup
   <!-------------------------------------------------------------------->**/

#ifndef AGGREGATE_BSDF_H_
#define AGGREGATE_BSDF_H_

#include <materials/BSDF.h>

namespace milton {

struct BSDFNode {
   BSDF  *bsdf;
   real_t pdf;
   
   BSDFNode(BSDF *bsdf_, real_t pdf_) 
      : bsdf(bsdf_), pdf(pdf_)
   { }
};

DECLARE_STL_TYPEDEF(std::vector<BSDFNode>, BSDFList);

class MILTON_DLL_EXPORT AggregateBSDF : public BSDF {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit AggregateBSDF(SurfacePoint &pt, Material *parent = NULL)
         : BSDF(pt, parent), m_bsdf(0)
      { }
      
      virtual ~AggregateBSDF();
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes this aggregate BSDF for sampling by selecting one 
       * of the possible child BSDFs
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual void setWi(const Vector3 &wi);
      
      virtual Event sample();
      
      virtual real_t getPd(const Event &event);
      
      virtual SpectralSampleSet evaluate(const Vector3 &wi, const Vector3 &wo);
      
      virtual bool isSpecular(Event &event) const;
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      BSDFList m_bsdfs;
      
      unsigned m_bsdf;
};

}

#endif // AGGREGATE_BSDF_H_

