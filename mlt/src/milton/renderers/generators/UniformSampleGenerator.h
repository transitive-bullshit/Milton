/**<!-------------------------------------------------------------------->
   @class  UniformSampleGenerator
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Uniform point sample generation over the unit interval, where samples 
   are generated uniformly in a grid over the entire domain
   <!-------------------------------------------------------------------->**/

#ifndef UNIFORM_SAMPLE_GENERATOR_H_
#define UNIFORM_SAMPLE_GENERATOR_H_

#include <renderers/generators/SampleGenerator.h>

namespace milton {

template <class SG>
class MILTON_DLL_EXPORT UniformSG : public SG {
   
   public:
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual void generate(PointSampleList &outSamples, 
                            const Viewport &domain);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // UNIFORM_SAMPLE_GENERATOR_H_

