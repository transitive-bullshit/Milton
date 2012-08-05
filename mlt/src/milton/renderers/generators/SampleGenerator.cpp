/**<!-------------------------------------------------------------------->
   @file   SampleGenerator.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Point sample generation over the 2D unit interval [0,1]^2
   
   @see Viewport
   @see SampleGeneratorThread
   <!-------------------------------------------------------------------->**/

#include "SampleGenerator.h"
#include "generators.h"
#include <QtCore/QtCore>

namespace milton {

void SampleGenerator::init()
{ }

void SampleGenerator::_addSample(const PointSample &s, 
                                 PointSampleList &samples)
{
   samples.push_back(s);
}

SampleGenerator *SampleGenerator::create(const std::string &type)
{
   if (type == "uniform" || type == "default") {
      return new UniformSampleGenerator();
   } else if (type == "stochastic") {
      return new StochasticSampleGenerator();
   } else if (type == "jittered") {
      return new JitteredSampleGenerator();
   } else if (type == "hilbert") {
      return new HilbertSampleGenerator();
   } else {
      ASSERT(0 && "encountered unknown sample type\n");
   }
   
   return NULL;
}

}

