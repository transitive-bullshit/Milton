/**<!-------------------------------------------------------------------->
   @file   JitteredSampleGenerator.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Jittered point sample generation over a given 2D domain where M*N 
   samples are generated randomly within M*N uniformly-spaced subdomains
   <!-------------------------------------------------------------------->**/

#include "JitteredSampleGenerator.h"
#include <QtCore/QtCore>
#include <Random.h>
using namespace std;

namespace milton {

template <class SG>
void JitteredSG<SG>::generate(PointSampleList &outSamples, 
                              const Viewport &viewport)
{
   const unsigned width   = viewport.getWidth();
   const unsigned height  = viewport.getHeight();
   
   const real_t binWidth  = viewport.getInvWidth();
   const real_t binHeight = viewport.getInvHeight();
   
   for(unsigned i = 0; i < height; ++i) {
      for(unsigned j = 0; j < width; ++j) {
         const real_t x = j * binWidth + Random::sample(0, binWidth);
         const real_t y = i * binWidth + Random::sample(0, binHeight);
         
         const bool update = (j == width - 1);
         SG::_addSample(
            PointSample(x, y, update, update && (i == height - 1)), outSamples
         );
      }
   }
}

}

// force explicit template specialization of JitteredSampleGenerator and 
// JitteredSampleGeneratorThread
#include <generators.h>

namespace milton {
   DECLARE_SAMPLE_GENERATOR(Jittered);
}

