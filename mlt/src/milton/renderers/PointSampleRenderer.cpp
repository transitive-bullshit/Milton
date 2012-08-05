/**<!-------------------------------------------------------------------->
   @file   PointSampleRenderer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstraction of renderers which construct their output by point sampling 
   a 2D domain (the film plane). The steps of generating point 
   samples across this domain (SampleGenerator), evaluating the samples 
   (PointSampleRenderer/SampleConsumer), and storing/using the evaluated 
   samples (RenderOutput) have been abstracted from each other.
   <!-------------------------------------------------------------------->**/

#include "PointSampleRenderer.h"
#include "RenderOutput.h"
#include "System.h"

#include <generators.h>
#include <QtCore/QtCore>
using namespace std;

namespace milton {

void PointSampleRenderer::render() {
   ASSERT(m_output);
   
   if (!m_initted)
      init();
   
   QMutexLocker lock(&m_renderMutex);
   m_output->setParent(this);
   m_output->init();
   
   // parse parameters
   unsigned noConsumers = getValue<unsigned>("noRenderThreads", System::getNoCPUs());
   ASSERT(noConsumers > 0);
   
   // initialize timer to begin counting
   m_timer.reset();
   
   cout << endl;
   cout << "rendering with " << noConsumers << 
      (noConsumers == 1 ? " thread" : " threads") << endl;
   
   // create and start sample generator
   SampleGeneratorThread *generator = _getGenerator();
   generator->init();
   generator->start();
   
   // create and start sample consumers (render threads)
   SampleConsumerList consumers;
   
   for(unsigned i = noConsumers; i--;) {
      SampleConsumer *consumer = _getConsumer();
      consumers.push_back(consumer);
      
      consumer->init();
      consumer->start();
   }
   
   //cout << "waiting on generator" << endl;
   
   // join with generator thread (wait until it has completed execution)
   while(!generator->wait());
   safeDelete(generator);
   
   // join with consumer threads (wait until they have all completed execution)
   for(unsigned i = noConsumers; i--;) {
      SampleConsumer *consumer = consumers[i];
      
      //cout << "waiting on consumer " << (noConsumers - i) << " / " << noConsumers << endl;
      while(!consumer->wait());
      
      safeDelete(consumer);
   }
   
   m_output->finalize();
   finalize();
   
   cout << endl << "done rendering in " << getElapsedTime() << endl << endl;
}

void PointSampleRenderer::sample(PointSample &outSample) {
   outSample.value.setValue(SpectralSampleSet::black());
}

void PointSampleRenderer::finalize()
{ }

void PointSampleRenderer::addSharedSample(const PointSample &s) {
   QMutexLocker lock(&m_mutex);
   ASSERT(m_noProducers > 0);
   
   while(m_sharedShamples.size() > MAX_SHARED_SAMPLES_SIZE)
      m_producer.wait(&m_mutex);
   
   m_sharedShamples.push_back(s);
   
   m_consumer.wakeOne();
}

void PointSampleRenderer::addSharedSamples(const PointSampleList &s) {
   QMutexLocker lock(&m_mutex);
   ASSERT(m_noProducers > 0);
   
   while(m_sharedShamples.size() >= MAX_SHARED_SAMPLES_SIZE)
      m_producer.wait(&m_mutex);
   
   FOREACH(PointSampleListConstIter, s, iter) {
      m_sharedShamples.push_back(*iter);
   }
   
   m_consumer.wakeAll();
}

bool PointSampleRenderer::getSharedSample(PointSample &outSample) {
   QMutexLocker lock(&m_mutex);
   
   while(m_sharedShamples.size() == 0) {
      if (m_sharedShamples.empty() && !m_noProducers)
         return false;
      
      m_consumer.wait(&m_mutex);
   }
   
   outSample = m_sharedShamples.front();
   m_sharedShamples.pop_front();
   
   m_producer.wakeAll();
   
   return true;
}

void PointSampleRenderer::addProducer() {
   QMutexLocker lock(&m_mutex);
   
   ++m_noProducers;
}

void PointSampleRenderer::removeProducer() {
   QMutexLocker lock(&m_mutex);
   
   ASSERT(0 < m_noProducers);
   
   if (0 == --m_noProducers) // completely done with sample generation
      m_consumer.wakeAll();
}

SampleGeneratorThread *PointSampleRenderer::_getGenerator() {
   const std::string &generator = getValue<std::string>("generator", 
                                                        std::string("default"));
   SampleGeneratorThread *sampleGenerator = 
      SampleGeneratorThread::create(generator);
   
   sampleGenerator->inherit(*this);
   sampleGenerator->setRenderer(this);
   sampleGenerator->setViewport(m_output->getViewport());
   
   return sampleGenerator;
}

SampleConsumer  *PointSampleRenderer::_getConsumer() {
   // TODO: consider making this customizable via parameters?
   
   return new SampleConsumer(this);
}

}

