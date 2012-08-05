/**<!-------------------------------------------------------------------->
   @class  RayTracer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Abstract ray tracing engine, with the following concrete 
   implementations:  WhittedRayTracer, StochasticRayTracer
   <!-------------------------------------------------------------------->**/

#ifndef RAY_TRACER_H_
#define RAY_TRACER_H_

#include <renderers/PointSampleRenderer.h>

namespace milton {

struct Ray;

class MILTON_DLL_EXPORT RayTracer : public PointSampleRenderer {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline RayTracer(RenderOutput *output = NULL, 
                       Camera *camera = NULL, 
                       Scene *scene = NULL)
         : PointSampleRenderer(output, camera, scene)
      { }
      
      virtual ~RayTracer()
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
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      /**
       * @brief
       *    Evaluates the radiance propogating in the opposite direction 
       * of the given ray using some type of illumination model
       *
       * @returns the spectral radiance emitted from the first surface 
       *    intersected along the given ray in the opposite direction from 
       *    the ray in the out param 'outRadiance'
       */
      virtual void _evaluate(const Ray &ray, SpectralSampleSet &outRadiance, 
                             PropertyMap &data) = 0;
};

}

#endif // RAY_TRACER_H_

