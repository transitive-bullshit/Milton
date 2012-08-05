/**<!-------------------------------------------------------------------->
   @class  PhotonMapper
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008

   @brief
      Implementation of Henrik Wann Jensen's photon mapping algorithm. Photon 
   Mapping is a two-pass global illumination algorithm in which photons are 
   first traced from light sources and stored in a global kd-tree called the 
   photon map. Final gathering is then performed by tracing paths from the eye 
   and using photons from the photon map to approximate irradiance at points 
   in the scene.
   
   @see Photon
   @see PhotonMap
   @see Jensen's book, "Realistic Image Synthesis Using Photon Mapping" (2001)
      for further details
   <!-------------------------------------------------------------------->**/

#ifndef PHOTON_MAPPER_H_
#define PHOTON_MAPPER_H_

#include <renderers/renderers/RayTracer.h>

class PhotonTracer;
class PhotonMap;

class PhotonMapper : public RayTracer {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline PhotonMapper(RenderOutput *output = NULL, 
                          Camera *camera = NULL, 
                          Scene *scene = NULL)
         : RayTracer(output, camera, scene), m_photonTracer(NULL), 
           m_diffusePhotonMap(NULL), m_causticPhotonMap(NULL)
      { }
      
      virtual ~PhotonMapper();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes photon maps
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      ///@name Forward tracing (photon tracing)
      //@{-----------------------------------------------------------------
      
      virtual void _tracePhotons();
      
      virtual void _tracePhoton(bool caustic = false);
      
      
      //@}-----------------------------------------------------------------
      ///@name Backward tracing (path / ray tracing)
      //@{-----------------------------------------------------------------
      
      virtual void _evaluate(const Ray &ray, SpectralSampleSet &outRadiance, 
                             PropertyMap &data);     
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      PhotonTracer *m_photonTracer;
      PhotonMap    *m_diffusePhotonMap;
      PhotonMap    *m_causticPhotonMap;
};

#endif // PHOTON_MAPPER_H_

