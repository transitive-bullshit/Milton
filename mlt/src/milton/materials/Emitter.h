/**<!-------------------------------------------------------------------->
   @class  Emitter
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract representation of emittance defined at a single point on a 
   surface in 3-space (describing a light).
   
      The reason Emitter subclasses BSDF is for convenience during generation 
   and evaluation of paths in bidirectional path tracing.  Emittance at a 
   point in a given direction, Le(x,wo), is broken into two parts, 
   Le0(x), and Le1(x), as originally detailed by Veach and Guibas
   (see Eric Veach's thesis, section 8.3.2).  Scattering at a vertex can then 
   be uniformly viewed as evaluation / sampling of a BSDF, including initial, 
   fake 'scattering' at a light source, which can be viewed as a special case 
   of scattering with no incident vector (the incident vector is thus 
   disregarded).
   
   @see Emitter::evaluate() or the Path class for more details.
   <!-------------------------------------------------------------------->**/

#ifndef EMITTER_H_
#define EMITTER_H_

#include <materials/BSDF.h>

namespace milton {

class MILTON_DLL_EXPORT Emitter : public BSDF {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   Emitter(SurfacePoint &pt, Sampler *sampler = NULL, 
                       Material *parent = NULL)
         : BSDF(pt, parent), m_pt(pt), m_parent(parent), m_sampler(sampler)
      { }
      
      virtual ~Emitter() {
         safeDelete(m_sampler);
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization that may be necessary before beginning 
       * to use this Emitter
       * 
       * @note default implementation initializes power from PropertyMap and 
       *    calculates radiant exitance
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns true iff this Emitter emits energy (is a light source)
       * 
       * @note if 'isEmitter' returns false, 'getLe' and 'getPower' must 
       *    return SpectralSampleSet::black
       */
      virtual bool isEmitter();
      
      /**
       * @returns the spectral radiance emitted along the given vector
       */
      virtual SpectralSampleSet getLe(const Vector3 &wo) = 0;
      
      /**
       * @returns the spectral power of this Emitter, representing the total 
       *    radiant energy emitted from surfaces associated with this Emitter
       *    (with units watts)
       * 
       * @note total Le emitted by integrating 'getLe' over the the sphere of 
       *    exitant directions and over the surface of the underlying shape 
       *    should equal the power of this emitter
       */
      virtual SpectralSampleSet getPower();
      
      /**
       * @returns the spectral radiant exitance (aka radiosity) of the 
       *    surface containing this Emitter, where radiant exitance is 
       *    defined as power per unit surface area (with units watts/m^2)
       */
      virtual SpectralSampleSet getRadiantExitance();
      
      /**
       * @returns Le0(x,0) which represents the total radiant exitance at this 
       *    point
       */
      virtual SpectralSampleSet getLe0();
      
      /**
       * @returns Le1(x,wo) which represents the directional distribution of 
       *    radiant exitance in the given direction
       */
      virtual SpectralSampleSet getLe1(const Vector3 &wo);
      
      /**
       * @brief
       *    Sets up OpenGL material state (light properties) to enable 
       * a crude/fast preview of this light in OpenGL
       *
       * @note default implementation places a GL pointlight at the center 
       *    of the bounding box of the given shape
       */
      virtual void preview(Shape *shape);
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Samples an exitent vector according to the underlying distribution 
       * of emitted radiance at the surface point
       * 
       * @returns the sampled vector
       */
      virtual Event sample(const Event &);
      
      /**
       * @returns the probability density of having sampled the given out 
       *    out vector 'event' with respect to whatever underlying sampling 
       *    strategy is being used to sample this Emitter
       */
      virtual real_t getPd(const Event &event);
      
      /**
       * @brief
       *    Mimics the behavior of a BSDF by separating emittance, Le(x,wo), 
       * into two parts: Le0(x) and Le1(x,wo).
       *    Le0(x) is independent of 'wo' and represents the total radiant 
       * exitance emitted at point x.
       *    Le1(x,wo) represents Le(x,w) / Le0(x), or in words, the 
       * directional distribution of emitted radiance at x, corresponding to 
       * the fraction of emitted radiance in direction 'wo' with respect to the
       * total radiant exitance emitted at point x.
       * 
       * @returns the directional distribution of emitted radiance in 
       *    direction 'wo'
       * @note all implementations should disregard 'wi', as it has no physical
       *    interpretation in the context of an Emitter
       * @note for details, see Eric Veach's thesis, section 8.3.2
       */
      virtual SpectralSampleSet evaluate(const Vector3 &wi, const Vector3 &wo);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors/Mutators
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the SurfacePoint at which this Emitter instance is defined
       */
      inline SurfacePoint &getSurfacePoint() {
         return m_pt;
      }
      
      /**
       * @returns the parent Material which instantiated this Emitter
       * 
       * @note the parent Material is also a PropertyMap and contians all 
       *    key-value inputs to this Emitter
       */
      inline Material *getParent() {
         return m_parent;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      SurfacePoint     &m_pt;
      Material         *m_parent;
      
      // total spectral radiance emitted (units watts)
      SpectralSampleSet m_power;
      
      // power per unit surface area (units watts/m^2)
      SpectralSampleSet m_radiantExitance;
      
      Sampler          *m_sampler;
};

}

#endif // EMITTER_H_

