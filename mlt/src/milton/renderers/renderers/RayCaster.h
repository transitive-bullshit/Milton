/**<!-------------------------------------------------------------------->
   @class  RayCaster
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Simple, non-recursive raycaster
   <!-------------------------------------------------------------------->**/

#ifndef RAY_CASTER_H_
#define RAY_CASTER_H_

#include <renderers/renderers/RayTracer.h>

namespace milton {

class MILTON_DLL_EXPORT RayCaster : public RayTracer {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline RayCaster(RenderOutput *output = NULL, 
                       Camera *camera = NULL, 
                       Scene *scene = NULL)
         : RayTracer(output, camera, scene), 
           m_ambient(SpectralSampleSet::fill(0.01))
      { }
      
      virtual ~RayCaster()
      { }
      
      //@}-----------------------------------------------------------------
      
      virtual void init();
      
   protected:
      virtual void _evaluate(const Ray &ray, SpectralSampleSet &outRadiance, 
                             PropertyMap &data);
      
   protected:
      SpectralSampleSet m_ambient;
};

}

#endif // RAY_CASTER_H_

