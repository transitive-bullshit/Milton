/**<!-------------------------------------------------------------------->
   @class  ReconstructionRenderOutput
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Records point samples from a renderer and attempts to reconstruct the 
   underlying image by filtering samples with a reconstruction filter
   <!-------------------------------------------------------------------->**/

#ifndef RECONSTRUCTION_RENDER_OUTPUT_H_
#define RECONSTRUCTION_RENDER_OUTPUT_H_

#include <renderers/RenderOutput.h>

namespace milton {

class MILTON_DLL_EXPORT ReconstructionRenderOutput : public RenderOutput {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      explicit ReconstructionRenderOutput(Image *output = NULL, 
                                          KernelFilter *filter = NULL);
      
      virtual ~ReconstructionRenderOutput();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual void addSample(PointSample &sample);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline KernelFilter *getFilter() {
         return m_filter;
      }
      
      inline void setFilter(KernelFilter *filter) {
         if (m_filter != filter) {
            KernelFilter *oldFilter = m_filter;
            
            m_filter = filter;
            
            if (m_filter != NULL)
               safeDelete(oldFilter);
         }
      }
      
      //@}-----------------------------------------------------------------
      
   protected:
      KernelFilter *m_filter;
};

}

#endif // RECONSTRUCTION_RENDER_OUTPUT_H_

