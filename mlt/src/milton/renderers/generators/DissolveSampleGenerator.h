/**<!-------------------------------------------------------------------->
   @class  DissolveSampleGenerator
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Cool dissolve effect which generates point samples uniformly over a 
   given 2D domain. The order in which the samples are generated looks random, 
   but it is actually quite deterministic and guaranteed to "visit" every 
   bucket in the domain.
   
   @see A Digital Dissolve Effect by Mike Morton
      (from "Graphics Gems", Academic Press, 1990)
   <!-------------------------------------------------------------------->**/

#ifndef DISSOLVE_SAMPLE_GENERATOR_H_
#define DISSOLVE_SAMPLE_GENERATOR_H_

#include <renderers/generators/SampleGenerator.h>

namespace milton {

template <class SG>
class MILTON_DLL_EXPORT DissolveSG : public SG {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline DissolveSG()
         : m_seq(1)
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@{-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual void generate(PointSampleList &outSamples, 
                            const Viewport &viewport);
      
      virtual bool generate(Point2 &pt, const Viewport &viewport);
      virtual bool generate(Point2 &pt, const Viewport &viewport, unsigned seq);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      unsigned _getBitWidth(unsigned n);
      
   protected:
      unsigned m_noSuperSamples;
      unsigned long m_mask;
      unsigned long m_seq;
};

}

#endif // DISSOLVE_SAMPLE_GENERATOR_H_

