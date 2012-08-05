/**<!-------------------------------------------------------------------->
   @class  EmitterSampler
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Sampler which selects a point on an emitter (light source) in the scene 
   with probability proportional to radiant exitance.
   <!-------------------------------------------------------------------->**/

#ifndef EMITTER_SAMPLER_H_
#define EMITTER_SAMPLER_H_

#include <stats/Sampler.h>

namespace milton {

class ShapeSet;
class Scene;

class MILTON_DLL_EXPORT EmitterSampler : public Sampler {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline EmitterSampler(Scene *scene)
         : Sampler(), m_scene(scene), m_lights(NULL), m_cdf(NULL), m_n(0)
      { }
      
      inline EmitterSampler(const EmitterSampler &copy)
         : Sampler(copy), m_scene(copy.m_scene), m_lights(copy.m_lights), 
           m_cdf(NULL), m_n(copy.m_n), m_area(copy.m_area)
      {
         m_cdf = new real_t[m_n];
         memcpy(m_cdf, copy.m_cdf, sizeof(real_t) * m_n);
      }
      
      virtual ~EmitterSampler() {
         safeDeleteArray(m_cdf);
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Should perform any initialization of this random variable which 
       * may be necessary to speed or prepare sampling
       * 
       * @note should be called before calling sample or getPd
       */
      virtual void init(ShapeSet *lights);
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a randomly chosen SurfacePoint, initialized to lie on an 
       *    Emitter; the returned event is of type SurfacePoint* and it is 
       *    up to the caller to ensure the returned SurfacePoint is deleted
       *    appropriately
       * @note the returned SurfacePoint is completely initialized, and 
       *    the actual light source that the sample lies on can be obtained 
       *    via the shape field of the returned SurfacePoint
       * @note consecutive calls to sample are expected to return 
       *    independent, identically distributed (IID) samples
       */
      virtual Event sample();
      
      /**
       * @returns the probability density with which the given event would be 
       *    sampled according to the underlying probability density function
       * @note the given event is assumed to lie within this random variable's
       *    sample space
       */
      virtual real_t getPd(const Event &event);
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the total surface area of all emitters in the scene
       */
      inline real_t getTotalSurfaceArea() const {
         return m_area;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      Scene    *m_scene;
      
      // contains all light sources in the scene
      ShapeSet *m_lights;
      
      // normalized array of relative powers of all n light sources
      real_t   *m_cdf;
      
      // total number of emitters
      unsigned  m_n;
      
      // total surface area of all emitters in the scene
      real_t    m_area;
};

}

#endif // EMITTER_SAMPLER_H_

