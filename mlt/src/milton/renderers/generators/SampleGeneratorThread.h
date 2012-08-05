/**<!-------------------------------------------------------------------->
   @class  SampleGeneratorThread
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Threaded PointSample generation over the 2D unit interval [0,1]^2
   (represents the 'producer' in the classic producer/consumer problem)
   
   @see Viewport
   @see SampleGenerator
   @see SampleConsumer
   <!-------------------------------------------------------------------->**/

#ifndef SAMPLE_GENERATOR_THREAD_H_
#define SAMPLE_GENERATOR_THREAD_H_

#include <renderers/generators/SampleGenerator.h>
#include <QtCore/QThread>

namespace milton {

class PointSampleRenderer;

class MILTON_DLL_EXPORT SampleGeneratorThread : public SampleGenerator, public QThread {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   SampleGeneratorThread(PointSampleRenderer *renderer = NULL)
         : SampleGenerator(), QThread(), 
           m_renderer(renderer)
      { }
      
      virtual ~SampleGeneratorThread();
      
      
      //@}-----------------------------------------------------------------
      ///@name Static factory
      //@{-----------------------------------------------------------------
      
      static SampleGeneratorThread *create(const std::string &type);
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * Generates samples over the unit domain in a new thread ('producer')
       */
      virtual void run();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline PointSampleRenderer *getRenderer() {
         return m_renderer;
      }
      
      void setRenderer(PointSampleRenderer *renderer);
      
      inline const Viewport &getViewport() const {
         return m_viewport;
      }
      
      inline void setViewport(const Viewport &v) {
         m_viewport = v;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _addSample(const PointSample &s, PointSampleList &samples);
      
   protected:
      Viewport             m_viewport;
      PointSampleRenderer *m_renderer;
};

}

#endif // SAMPLE_GENERATOR_THREAD_H_

