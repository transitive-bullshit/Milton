/**<!-------------------------------------------------------------------->
   @class  BSDF
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract representation of a BSDF defined at a single point on a 
   surface in 3-space.
   
      A BSDF (or Bidirectional Scattering Distribution Function) is a function 
   defining the fraction of light propagated after hitting a surface in a 
   given exitent direction, wo, from a given incident direction, wi. It 
   represents the observed radiance leaving in direction wo, per unit of 
   irradiance arriving from wi. A BSDF is commonly denoted by the notation 
   fs(wi->wo).
      It is much more common in computer graphics to hear one speak about 
   BRDFs (or Bidirectional Reflectance Distribution Functions), the difference 
   being that BSDFs are defined over the entire sphere of solid angles and 
   therefore include transmission (transparency), whereas BRDFs are defined 
   only on the positive hemisphere at a surface point with respect to its 
   local geometric normal.  BRDFs represent a subset of allowable BSDFs, and 
   they suffice in simulating the majority of real world materials. The 
   added complexity / generality of BSDFs, however, is important to any 
   physically based rendering engine such as Milton. A BRDF is commonly 
   denoted by fr(wi->wo).
   
      Physically valid BRDFs have several basic properties or consraints, 
   namely reciprocity and conservation of energy.
      Reciprocity means that BRDFs are symmetric: fr(wi->wo) = fr(wo->wi). 
   For this reason, it is common to make the symmetric explicit in the notation
   by instead writing fr(wi<->wo) in the case of BRDFs or fs(wi<->wo) in the 
   case of BSDFs.
      Conservation of energy states that a surface should not reflect more 
   energy than it receives. Formally this can be stated as the integral of 
   fr(wi->wo) over the positive hemisphere at any point with respect to a 
   projected solid angle measure has to be less than or equal to one for 
   all possible incident vectors, wi.
   
   @htmlonly
   Example usage:
   <pre><code>
      SurfacePoint pt;
      const real_t t = m_scene->getIntersection(ray, pt);
      
      // lazily initialize SurfacePoint and return if no intersection
      if (!pt.init(ray, t))
         return; // ray didn't hit anything (t == INFINITY)
      
      // sample the BSDF for an exitant direction
      const Event &event = pt.bsdf->sample();
      const Vector3 &wo  = event;
      
      if (wo == Vector3::zero())
         return; // invalid exitant direction
      
      // evaluate BSDF in the given exitant direction and divide by probability
      // with which we sampled that direction
      const real_t pdf = pt.bsdf->getPd(event);
      const SpectralSampleSet &fs = pt.bsdf->evaluate(wo) / pdf;
      
      // ... trace another ray in direction 'wo' ...
   </code></pre>
   @endhtmlonly
   <!-------------------------------------------------------------------->**/

#ifndef BSDF_H_
#define BSDF_H_

#include <core/SurfacePoint.h>
#include <utils/SpectralSampleSet.h>
#include <stats/Sampler.h>
#include <shapes/Shape.h>
#include <common/image/Image.h>

namespace milton {

class Material;

class MILTON_DLL_EXPORT BSDF : public Sampler, public SSEAligned {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit BSDF(SurfacePoint &pt, Material *parent = NULL)
         : Sampler(), m_wi(1, 0, 0), m_pt(pt), m_parent(parent)
      { }
      
      virtual ~BSDF()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization that may be necessary before beginning 
       * to sample this BSDF
       * 
       * @note default implementation is empty
       */
      virtual void init()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Sets the incident vector, wi, for this BSDF. All BSDF sampling 
       * is with respect to the current value of wi.
       */
      virtual void setWi(const Vector3 &wi) {
         m_wi = wi;
      }
      
      /**
       * @brief
       *    Samples an exitent vector at the given surface point
       * 
       * @returns the sampled vector as a Vector3 wrapped in an Event object
       * @note returns Vector3::zero() if no exitent vector was sampled
       *    (to model a certain probability of absorption, for instance)
       * 
       * @htmlonly
       * Example usage:
       * <pre><code>
       *    const Event &event = bsdf.sample();
       *    const Vector3 &wo  = event.getValue<Vector3>();
       *    const real_t pd    = bsdf.getPd(event);        // p(x) probability
       *    const SpectralSampleSet &fs = bsdf.evaluate(wo); // f(x) reflectivity
       * </code></pre>
       * @endhtmlonly
       */
      virtual Event sample() {
         Event e;
         return sample(e);
      }
      
      /**
       * TODO: document
       * 
       * passed in event is for sampling the same type of scattering as a 
       * previous invocation of sample() -- eg, so once you get a specular 
       * bounce you can request more specular samples
       */
      virtual Event sample(const Event &event) = 0;
      
      /**
       * @returns the probability density of having sampled the given out 
       *    out vector @p event with respect to whatever underlying sampling 
       *    strategy is being used to sample the BSDF
       * 
       * @note probability density is assumed to be with respect to projected 
       *    solid angle (the conversion from solid angle to projected solid 
       *    angle involves dividing the solid angle density by the absolute 
       *    value of the cosine of the angle between the local surface normal 
       *    and the exitant vector in @p event)
       */
      virtual real_t getPd(const Event &event) = 0;
      
      /**
       * @brief
       *    Evaluates the spectral BSDF at the given surface point with respect 
       * to the out vector, 'wo', at the underlying surface point and given 
       * incident vector, 'wi'
       * 
       * @returns the spectral radiance leaving in direction wo, per unit of 
       *    irradiance arriving from wi (fraction in [0, 1] with units sr^-1)
       * 
       * @note uses the currently set m_wi for the incident vector
       */
      inline SpectralSampleSet evaluate(const Vector3 &wo) {
         return evaluate(m_wi, wo);
      }
      
      /**
       * @brief
       *    Evaluates the spectral BSDF at the given surface point with respect 
       * to the out vector, 'wo', at the underlying surface point and given 
       * incident vector, 'wi'
       * 
       * @returns the spectral radiance leaving in direction wo, per unit of 
       *    irradiance arriving from wi (fraction in [0, 1] with units sr^-1)
       */
      virtual SpectralSampleSet evaluate(const Vector3 &wi, 
                                        const Vector3 &wo) = 0;
      
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
      inline bool isSpecular() const {
         Event e;
         return isSpecular(e);
      }
      
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
       * @note implementations are free to use the given Event's metadata 
       *    to determine whether or not it represents a specular event
       */
      virtual bool isSpecular(Event &/* event unused*/) const {
         return false;
      }
      
      /**
       * @brief
       *    Sets up OpenGL material state (color properties, etc.) to enable 
       * a crude/fast preview of geometry to which this BSDF is applied
       *
       * @note default implementation sets the current glColor
       * @param shape is the Shape which this BSDF will preview itself on
       */
      virtual void preview(Shape *shape);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors/Mutators
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the SurfacePoint at which this BSDF instance is defined
       */
      inline SurfacePoint &getSurfacePoint() {
         return m_pt;
      }
      
      /**
       * @returns the current incident vector, wi, for which this BSDF will 
       *    be sampled
       */
      inline const Vector3 &getWi() const {
         return m_wi;
      }
      
      /**
       * @returns the parent Material which instantiated this BSDF
       * 
       * @note the parent Material is also a PropertyMap and contians all 
       *    key-value inputs to this BSDF
       */
      inline Material *getParent() {
         return m_parent;
      }
      
      /**
       * @returns the perfect specularly reflected direction given the current
       *    incident vector, wi, which is defined to be wi rotated about the 
       *    local surface normal
       * 
       * @see also Vector::reflectVector
       */
      inline Vector3 getSpecularReflection() const {
         return m_wi.reflectVector(m_pt.normalS);
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      Vector3       m_wi;
      SurfacePoint &m_pt;
      Material     *m_parent;
};

}

#endif // BSDF_H_

