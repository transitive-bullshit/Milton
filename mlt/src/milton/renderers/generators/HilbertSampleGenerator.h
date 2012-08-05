/**<!-------------------------------------------------------------------->
   @class  HilbertSampleGenerator
   @author Travis Fischer (fisch0920@gmail.com)
   @date   December 2006
   
   @brief
      Recursive space-filling L-System represented graphically
   
   @see http://en.wikipedia.org/wiki/Hilbert_curve
   <!-------------------------------------------------------------------->**/

#ifndef HILBERT_SAMPLE_GENERATOR_H_
#define HILBERT_SAMPLE_GENERATOR_H_

#include <renderers/generators/SampleGenerator.h>
#include <renderers/generators/LSystem.h>

namespace milton {

template <class SG>
class MILTON_DLL_EXPORT HilbertSG : public SG, public LSystem {
   
   public:
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      inline HilbertSG()
         : LSystem("LR", "L")
      { }
      
      virtual ~HilbertSG()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual void generate(PointSampleList &outSamples, 
                            const Viewport &viewport);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual std::string _processRule(char c);
};

}

#endif // HILBERT_SAMPLE_GENERATOR_H_

