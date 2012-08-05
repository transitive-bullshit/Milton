/**<!-------------------------------------------------------------------->
   @file   StochasticSampleGenerator.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Stochastic point sample generation over a given 2D domain where samples 
   are generated completely randomly over the entire domain
   <!-------------------------------------------------------------------->**/

#include "StochasticSampleGenerator.h"
#include <Random.h>
#include <QtCore/QtCore>

namespace milton {

template <class SG>
void StochasticSG<SG>::generate(PointSampleList &outSamples, 
                                const Viewport &viewport)
{
   const unsigned width = viewport.getWidth();
   unsigned noSamples   = width * viewport.getHeight();
   
   while(noSamples--) {
      const real_t x = Random::sample();
      const real_t y = Random::sample();
      
      SG::_addSample(
         PointSample(x, y, (noSamples % width) == 0, !noSamples), outSamples
      );
   }
}

}

// force explicit template specialization of StochasticSampleGenerator and 
// StochasticSampleGeneratorThread
#include <generators.h>

namespace milton {
   DECLARE_SAMPLE_GENERATOR(Stochastic);
}

