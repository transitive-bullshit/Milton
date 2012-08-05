/**<!-------------------------------------------------------------------->
   @class  Sensor
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Representation of importance defined at a single point on a surface in 
   3-space (describing a sensor / camera).
   
      The reason Sensor subclasses BSDF is for convenience during generation 
   and evaluation of paths in bidirectional path tracing.  Importance at a 
   point in a given direction, We(x,wo), is broken into two parts, 
   We0(x), and We1(x,wo), as originally detailed by Veach and Guibas
   (see Eric Veach's thesis, section 8.3.2).  Scattering at a vertex can then 
   be uniformly viewed as evaluation / sampling of a BSDF, including initial, 
   fake 'scattering' at a sensor, which can be viewed as a special case 
   of scattering with no exitent vector (the exitent vector is thus 
   disregarded).
   
   @see Sensor::evaluate() or the Path class for more details.
   <!-------------------------------------------------------------------->**/

#ifndef SENSOR_H_
#define SENSOR_H_

#include <materials/BSDF.h>

namespace milton {

class Camera;

class MILTON_DLL_EXPORT Sensor : public BSDF {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   Sensor(SurfacePoint &pt, Material *parent = NULL)
         : BSDF(pt, parent), m_pt(pt), m_parent(parent), m_camera(NULL)
      { }
      
      virtual ~Sensor() 
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization that may be necessary before beginning 
       * to use this sensor
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns true iff this sensor emits importance (is a valid sensor)
       * 
       * @note if 'isSensor' returns false, 'getWe' and 'getImportance' must 
       *    return SpectralSampleSet::black
       */
      virtual bool isSensor();
      
      /**
       * @returns the spectral importance emitted along the given vector
       */
      virtual SpectralSampleSet getWe(const Vector3 &wo);
      
      /**
       * @returns We0(x,0) which represents the total importance emitted at 
       *    this point
       */
      virtual SpectralSampleSet getWe0();
      
      /**
       * @returns We1(x,wo) which represents the directional distribution of 
       *    importance emitted in the given direction
       */
      virtual SpectralSampleSet getWe1(const Vector3 &wo);
      
      /**
       * @returns the spectral importance of this sensor, representing the 
       *    total radiant energy emitted from surfaces associated with this 
       *    sensor; TODO: what units?
       * 
       * @note total We emitted by integrating 'getWe' over the the sphere of 
       *    exitant directions should equal the importance of this sensor
       */
      virtual SpectralSampleSet getImportance();
      
      /**
       * @note implementation is empty
       */
      virtual void preview(Shape *shape)
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Samples an exitent vector according to the underlying distribution 
       * of emitted importance at the surface point
       * 
       * @returns the sampled vector
       */
      virtual Event sample(const Event &);
      
      /**
       * @returns the probability density of having sampled the given out 
       *    out vector 'event' with respect to whatever underlying sampling 
       *    strategy is being used to sample this sensor
       */
      virtual real_t getPd(const Event &event);
      
      /**
       * @brief
       *    Mimics the behavior of a BSDF by separating emittance, We(x,wo), 
       * into two parts: We0(x) and We1(x,wo).
       *    We0(x) is independent of 'wo' and represents the total 
       * importance emitted at point x.
       *    We1(x,wo) represents We(x,w) / We0(x), or in words, the 
       * directional distribution of importance emitted at x, corresponding to 
       * the fraction of importance emitted in direction 'wo' with respect to the
       * total importance emitted at point x.
       * 
       * @returns the directional distribution of importance emitted in 
       *    direction 'wo'
       * @note all implementations should disregard 'wo', as it has no physical
       *    interpretation in the context of a sensor
       * @note for details, see Eric Veach's thesis, section 8.3.2
       */
      virtual SpectralSampleSet evaluate(const Vector3 &wi, const Vector3 &wo);
      
      /**
       * @returns true iff this BSDF is non-zero over a set of solid angles 
       *    with measure zero (measured with respect to solid angle)
       * 
       * @note a perfectly specular material has a dirac distribution for its 
       *    reflectance, and therefore needs special consideration when 
       *    sampling the BRDF for simulation purposes
       * @note the default implementation returns false because perfectly
       *    specular surfaces don't occur that often in real life, though they 
       *    abound in computer graphics (due to their ease of simulation)
       */
      virtual bool isSpecular(Event &/* event unused*/) const {
         return true;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors/Mutators
      //@{-----------------------------------------------------------------
      
      inline SurfacePoint &getSurfacePoint() {
         return m_pt;
      }
      
      inline Material *getParent() {
         return m_parent;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      SurfacePoint &m_pt;
      Material    *m_parent;
      
      Camera      *m_camera;
};

}

#endif // SENSOR_H_

