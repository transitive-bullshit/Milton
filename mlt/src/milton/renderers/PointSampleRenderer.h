/**<!-------------------------------------------------------------------->
   @class  PointSampleRenderer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstraction of renderers which construct their output by point sampling 
   a 2D domain (the film plane). The steps of generating point 
   samples across this domain (SampleGenerator), evaluating the samples 
   (PointSampleRenderer/SampleConsumer), and storing/using the evaluated 
   samples (RenderOutput) have been abstracted from each other.
   <!-------------------------------------------------------------------->**/

#ifndef POINT_SAMPLE_RENDERER_H_
#define POINT_SAMPLE_RENDERER_H_

#include <renderers/Renderer.h>
#include <renderers/PointSample.h>

#include <QtCore/QWaitCondition>
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>
#include <QtCore/QMutex>

#define MAX_SHARED_SAMPLES_SIZE     (512)

namespace milton {

struct PointSample;
class  RenderOutput;

class  SampleGeneratorThread;
class  SampleConsumer;

DECLARE_STL_TYPEDEF(std::vector<SampleConsumer *>, SampleConsumerList);
      
class MILTON_DLL_EXPORT PointSampleRenderer : public Renderer {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline PointSampleRenderer(RenderOutput *output = NULL, 
                                 Camera *camera = NULL, 
                                 Scene *scene   = NULL)
         : Renderer(camera, scene), m_noProducers(0), m_output(output)
      { }
      
      virtual ~PointSampleRenderer()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Renders the underlying scene synchronously
       */
      virtual void render();
      
      /**
       * @brief
       *    Renders a single point sample (incident radiance evaluation) at 
       * the point specified on the film plane
       * 
       * @note this is the main method that specific PointSampleRenderers 
       *    should override
       */
      virtual void sample(PointSample &outSample);
      
      /**
       * @brief
       *    Called upon finishing a call to render
       * 
       * @note
       *    Default implementation is blank
       */
      virtual void finalize();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      virtual RenderOutput *getOutput() {
         return m_output;
      }
      
      virtual void setOutput(RenderOutput *output) {
         QMutexLocker lock(&m_renderMutex);
         
         m_output = output;
      }
      
      
      //@}-----------------------------------------------------------------
      
   public:
      virtual void addSharedSample (const PointSample &s);
      virtual void addSharedSamples(const PointSampleList &s);
      virtual bool getSharedSample(PointSample &outSample);
      
      virtual void addProducer();
      virtual void removeProducer();
      
   protected:
      /**
       * @brief
       *    Factory method for creating new SampleGeneratorThreads
       */
      virtual SampleGeneratorThread *_getGenerator();
      
      /**
       * @brief
       *    Factory method for creating new SampleConsumers
       */
      virtual SampleConsumer  *_getConsumer();
      
   protected:
      /// Provides mutual exclusion to 'render' method
      QMutex              m_renderMutex;
      
      /// Provides mutual exclusion to shared sample list betweeen 
      /// SampleGeneratorThreads and SampleConsumers
      QMutex              m_mutex;
      
      /// Wait condition variables used to implement producer/consumer 
      /// with respect to shared PointSampleQueue
      QWaitCondition      m_producer;
      QWaitCondition      m_consumer;
      
      PointSampleQueue    m_sharedShamples;
      unsigned            m_noProducers;
      
      /// Abstract class which aggregates point samples (eg, an Image wrapper)
      RenderOutput       *m_output;
};

}

#endif // POINT_SAMPLE_RENDERER_H_

