/**<!-------------------------------------------------------------------->
   @class  SuperSampleGenerator
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
     Brute force super sampling, where sub-bin (sub-pixel) sampling is done 
   via an auxillary SampleGenerator
   <!-------------------------------------------------------------------->**/

#ifndef SUPER_SAMPLE_GENERATOR_H_
#define SUPER_SAMPLE_GENERATOR_H_

#include <renderers/generators/SampleGenerator.h>

namespace milton {

DECLARE_STL_TYPEDEF(std::vector<PointSampleList>, PointSampleListList);

template <class SG>
class MILTON_DLL_EXPORT SuperSG : public SG {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline SuperSG(const std::string &subGeneratorType)
         : SG(), m_subGeneratorType(subGeneratorType)
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual void generate(PointSampleList &outSamples, 
                            const Viewport &viewport);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      std::string         m_subGeneratorType;
      PointSampleListList m_samples;
      unsigned            m_noSuperSamples;
};

}

#endif // SUPER_SAMPLE_GENERATOR_H_

