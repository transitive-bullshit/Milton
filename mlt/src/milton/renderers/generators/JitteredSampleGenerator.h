/**<!-------------------------------------------------------------------->
   @class  JitteredSampleGenerator
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Jittered point sample generation over a given 2D domain where M*N 
   samples are generated randomly within M*N uniformly-spaced subdomains
   <!-------------------------------------------------------------------->**/

#ifndef JITTERED_SAMPLE_GENERATOR_H_
#define JITTERED_SAMPLE_GENERATOR_H_

#include <renderers/generators/SampleGenerator.h>

namespace milton {

template <class SG>
class MILTON_DLL_EXPORT JitteredSG : public SG {
   
   public:
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual void generate(PointSampleList &outSamples, 
                            const Viewport &viewport);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // JITTERED_SAMPLE_GENERATOR_H_

