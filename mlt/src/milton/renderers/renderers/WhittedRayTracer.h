/**<!-------------------------------------------------------------------->
   @class  WhittedRayTracer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Classic Whitted-style raytracer; for a concise introduction to 
   raytracing, see http://www.raytracing.co.uk/study/ray_intro.htm
   <!-------------------------------------------------------------------->**/

#ifndef WHITTED_RAY_TRACER_H_
#define WHITTED_RAY_TRACER_H_

#include <renderers/renderers/RayTracer.h>

namespace milton {

class MILTON_DLL_EXPORT WhittedRayTracer : public RayTracer {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline WhittedRayTracer(RenderOutput *output = NULL, 
                              Camera *camera = NULL, 
                              Scene *scene = NULL)
         : RayTracer(output, camera, scene), m_noIndirectSamples(1), 
           m_maxDepth(5), m_ambient(SpectralSampleSet::fill(0.01))
      { }
      
      virtual ~WhittedRayTracer()
      { }
      
      //@}-----------------------------------------------------------------
      
      virtual void init();
      
   protected:
      virtual void _evaluate(const Ray &ray, SpectralSampleSet &outRadiance, 
                             PropertyMap &data);
      
   protected:
      unsigned m_noIndirectSamples;
      unsigned m_maxDepth;
      
      SpectralSampleSet m_ambient;
};

}

#endif // WHITTED_RAY_TRACER_H_

