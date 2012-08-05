/**<!-------------------------------------------------------------------->
   @class  DirectIllumination
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Interface for estimating direct illumination from all luminaires in 
   a scene to a given surface point on a surface
   <!-------------------------------------------------------------------->**/

#ifndef DIRECT_ILLUMINATION_H_
#define DIRECT_ILLUMINATION_H_

#include <utils/PropertyMap.h>
#include <utils/SpectralSampleSet.h>

namespace milton {

class  Renderer;
class  SampleGenerator;
struct SurfacePoint;

class MILTON_DLL_EXPORT DirectIllumination : public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline DirectIllumination(Renderer *renderer)
         : m_renderer(renderer)
      { }
      
      virtual ~DirectIllumination();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization routines
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization which may be necessary before 
       * beginning to sample direct illumination
       * 
       * @note
       *    Default implementation initializes several values from the 
       * PropertyMap
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Attempts to estimate the direct illumination contribution from 
       * all emitters in the scene to the given surface point
       */
      virtual SpectralSampleSet evaluate(SurfacePoint &pt);
      
      /**
       * @brief
       *    Attempts to estimate the direct illumination contribution from 
       * all emitters in the scene to the given surface point
       */
      virtual SpectralSampleSet evaluate(SurfacePoint &pt, unsigned noDirectSamples);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline SampleGenerator *getSampleGenerator() {
         return m_generator;
      }
      
      inline unsigned getNoDirectSamples() {
         return m_noDirectSamples;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      Renderer        *m_renderer;
      
      SampleGenerator *m_generator;
      unsigned         m_noDirectSamples;
};

}

#endif // DIRECT_ILLUMINATION_H_

