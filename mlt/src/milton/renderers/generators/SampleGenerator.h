/**<!-------------------------------------------------------------------->
   @class  SampleGenerator
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Point sample generation over the 2D unit interval [0,1]^2
   
   @see Viewport
   @see SampleGeneratorThread
   <!-------------------------------------------------------------------->**/

#ifndef SAMPLE_GENERATOR_H_
#define SAMPLE_GENERATOR_H_

#include <renderers/PointSample.h>
#include <utils/PropertyMap.h>
#include <core/Viewport.h>

namespace milton {

class MILTON_DLL_EXPORT SampleGenerator : public PropertyMap {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline SampleGenerator()
      { }
      
      virtual ~SampleGenerator()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Static factory
      //@{-----------------------------------------------------------------
      
      static SampleGenerator *create(const std::string &type);
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization which may be necessary before 
       * beginning to generate samples
       * 
       * @note
       *    Default implementation is empty
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Generates samples over the unit square [0,1]^2 with regards to 
       * the given viewport and stores them in outSamples
       * 
       * @note
       *    Samples will be appended to outSamples if outSamples is not 
       * initially empty
       */
      virtual void generate(PointSampleList &outSamples, const Viewport &v) = 0;
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      /**
       * @brief
       *    Main interface for subclasses to add a given PointSample to be 
       * 'generated'. Default implementation is simple: samples.push_back(s);
       * Subclasses should not access samples list directly but should 
       * instead use this method which allows for SampleGeneratorThread to 
       * override _addSample to instead notify a PointSampleRenderer's 
       * shared pool of pending work
       */
      virtual void _addSample(const PointSample &s, PointSampleList &samples);
};

}

#endif // SAMPLE_GENERATOR_H_

