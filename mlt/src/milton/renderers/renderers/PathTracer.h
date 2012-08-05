/**<!-------------------------------------------------------------------->
   @class  PathTracer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Unbiased path tracer with support for efficient direct illumination
   <!-------------------------------------------------------------------->**/

#ifndef PATH_TRACER_H_
#define PATH_TRACER_H_

#include <renderers/renderers/RayTracer.h>

namespace milton {

class MILTON_DLL_EXPORT PathTracer : public RayTracer {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline PathTracer(RenderOutput *output = NULL, 
                        Camera *camera = NULL, 
                        Scene *scene = NULL)
         : RayTracer(output, camera, scene)
      { }
      
      virtual ~PathTracer()
      { }
      
      
      //@}-----------------------------------------------------------------
      
      virtual void init();
      
   protected:
      virtual void _evaluate(const Ray &ray, SpectralSampleSet &outRadiance, 
                             PropertyMap &data);
      
   protected:
      bool m_efficientDirect;
};

}

#endif // PATH_TRACER_H_

