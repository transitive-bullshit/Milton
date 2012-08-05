/**<!-------------------------------------------------------------------->
   @class  StochasticSampleGenerator
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Stochastic point sample generation over a given 2D domain where samples 
   are generated completely randomly over the entire domain
   <!-------------------------------------------------------------------->**/

#ifndef STOCHASTIC_SAMPLE_GENERATOR_H_
#define STOCHASTIC_SAMPLE_GENERATOR_H_

#include <renderers/generators/SampleGenerator.h>

namespace milton {

template <class SG>
class MILTON_DLL_EXPORT StochasticSG : public SG {
   
   public:
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual void generate(PointSampleList &outSamples, 
                            const Viewport &viewport);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // STOCHASTIC_SAMPLE_GENERATOR_H_

