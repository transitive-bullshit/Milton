/**<!-------------------------------------------------------------------->
   @class  DefaultRenderOutput
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Records point samples from a renderer and reconstructs the underlying 
   image, using a default reconstruction reconstruction filter
   <!-------------------------------------------------------------------->**/

#ifndef DEFAULT_RENDER_OUTPUT_H_
#define DEFAULT_RENDER_OUTPUT_H_

#include <renderers/outputs/ReconstructionRenderOutput.h>

namespace milton {

class MILTON_DLL_EXPORT DefaultRenderOutput : public ReconstructionRenderOutput {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      DefaultRenderOutput(Image *output = NULL);
      
      virtual ~DefaultRenderOutput()
      { }
};

}

#endif // DEFAULT_RENDER_OUTPUT_H_

