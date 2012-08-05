/**<!-------------------------------------------------------------------->
   @file   SampleGeneratorThread.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Threaded PointSample generation over the 2D unit interval [0,1]^2
   (represents the 'producer' in the classic producer/consumer problem)
   
   @see Viewport
   @see SampleGenerator
   @see SampleConsumer
   <!-------------------------------------------------------------------->**/

#include "SampleGeneratorThread.h"
#include "generators.h"

#include <PointSampleRenderer.h>
#include <QtCore/QtCore>

namespace milton {

SampleGeneratorThread::~SampleGeneratorThread() {
   ASSERT(m_renderer);
   
   m_renderer->removeProducer();
}

void SampleGeneratorThread::run() {
   PointSampleList dummyList;
   ASSERT(m_renderer);
   
   generate(dummyList, m_viewport);
}

void SampleGeneratorThread::_addSample(const PointSample &s, 
                                       PointSampleList &/* unused */)
{
   m_renderer->addSharedSample(s);
}

void SampleGeneratorThread::setRenderer(PointSampleRenderer *renderer) {
   ASSERT(renderer);
   
   m_renderer = renderer;
   m_renderer->addProducer();
}

SampleGeneratorThread *SampleGeneratorThread::create(const std::string &type) {
   if (type == "uniform") {
      return new UniformSampleGeneratorThread();
   } else if (type == "stochastic") {
      return new StochasticSampleGeneratorThread();
   } else if (type == "jittered") {
      return new JitteredSampleGeneratorThread();
   } else if (type == "dissolve") {
      return new DissolveSampleGeneratorThread();
   } else if (type == "hilbert") {
      return new HilbertSampleGeneratorThread();
   } else if (type == "super" || type == "default") {
      return new SuperSampleGeneratorThread("jittered");
   } else {
      ASSERT(0 && "encountered unknown sample type\n");
   }
   
   return NULL;
}

}

