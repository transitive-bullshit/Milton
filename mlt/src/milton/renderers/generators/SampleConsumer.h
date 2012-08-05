/**<!-------------------------------------------------------------------->
   @class  SampleConsumer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Threaded point sample evaluation (represents the 'consumer' in the 
   classic producer/consumer problem)
   
   @see also SampleGenerator
   <!-------------------------------------------------------------------->**/

#ifndef SAMPLE_CONSUMER_H_
#define SAMPLE_CONSUMER_H_

#include <common/common.h>
#include <QtCore/QThread>

namespace milton {

class PointSampleRenderer;

class MILTON_DLL_EXPORT SampleConsumer : public QThread {
   public:
      inline SampleConsumer(PointSampleRenderer *renderer) 
         : m_renderer(renderer)
      { }
      
      virtual ~SampleConsumer()
      { }
      
      virtual void init();
      
      virtual void run();
      
   protected:
      PointSampleRenderer *m_renderer;
};

}

#endif // SAMPLE_CONSUMER_H_

