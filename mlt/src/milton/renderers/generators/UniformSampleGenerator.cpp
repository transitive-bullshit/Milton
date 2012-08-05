/**<!-------------------------------------------------------------------->
   @file   UniformSampleGenerator.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Uniform point sample generation over the unit interval, where samples 
   are generated uniformly in a grid over the entire domain
   <!-------------------------------------------------------------------->**/

#include "UniformSampleGenerator.h"
#include <QtCore/QtCore>

namespace milton {

template <class SG>
void UniformSG<SG>::generate(PointSampleList &outSamples, 
                             const Viewport &domain)
{
   const unsigned width   = domain.getWidth();
   const unsigned height  = domain.getHeight();
   
   const real_t binWidth  = domain.getInvWidth();
   const real_t binHeight = domain.getInvHeight();
   
   const real_t binWidthDiv2  = binWidth  * 0.5;
   const real_t binHeightDiv2 = binHeight * 0.5;
   
   for(unsigned i = 0; i < height; ++i) {
      for(unsigned j = 0; j < width; ++j) {
         const real_t x = binWidthDiv2  + j * binWidth;
         const real_t y = binHeightDiv2 + i * binHeight;
         
         const bool update = (j == width - 1);
         SG::_addSample(
            PointSample(x, y, update, update && (i == height - 1)), 
            outSamples
         );
      }
   }
}

}

// force explicit template specialization of UniformSampleGenerator and 
// UniformSampleGeneratorThread
#include <generators.h>

namespace milton {
   DECLARE_SAMPLE_GENERATOR(Uniform);
}

