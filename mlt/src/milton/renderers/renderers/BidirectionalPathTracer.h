/**<!-------------------------------------------------------------------->
   @class  BidirectionalPathTracer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Unbiased bidirectional path tracer with support for efficient direct 
   illumination
   <!-------------------------------------------------------------------->**/

#ifndef BIDIRECTIONAL_PATH_TRACER_H_
#define BIDIRECTIONAL_PATH_TRACER_H_

#include <renderers/PointSampleRenderer.h>
#include <renderers/utils/IPathGenerator.h>
#include <filters/ProgressiveFilterValue.h>

namespace milton {

class HDRImage;

class MILTON_DLL_EXPORT BidirectionalPathTracer : public PointSampleRenderer, 
                                                  public IPathGenerator
{
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline BidirectionalPathTracer(RenderOutput *output = NULL, 
                                     Camera *camera = NULL, 
                                     Scene *scene = NULL)
         : PointSampleRenderer(output, camera, scene), m_images(NULL)
      { }
      
      virtual ~BidirectionalPathTracer()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Renders a single point sample (incident radiance evaluation) at 
       * the point specified on the film plane
       */
      virtual void sample(PointSample &outSample);
      
      /**
       * @brief
       *    Called upon finishing a call to render
       */
      virtual void finalize();
      
      
      //@}-----------------------------------------------------------------
      ///@name Path generation
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Generates a complete, valid path from scratch
       * 
       * @note path generated is assumed to start at an emitter (light) and 
       *    end at a sensor (camera)
       * @returns true on success or false on failure (if an invalid path 
       *    was encountered)
       */
      virtual bool generate(Path &outPath);
      
      /**
       * @brief
       *    Generates a light subpath which is assumed to start at an emitter
       */
      virtual void generateL(Path &outPath);
      
      /**
       * @brief
       *    Generates an eye subpath which is assumed to end at a sensor
       */
      virtual void generateE(Path &outPath);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      HDRImage **m_images;
      ProgressiveFilterValue<SpectralSampleSet> **m_filters;
};

}

#endif // BIDIRECTIONAL_PATH_TRACER_H_

