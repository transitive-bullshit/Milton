/**<!-------------------------------------------------------------------->
   @file   BSDF.h
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

#include "BSDF.h"
#include <materials.h>

#include <GL/gl.h>

namespace milton {

void BSDF::preview(Shape *) {
   SpectralSampleSet spectrum;
   
   // be careful; we don't want to add any keys to the PropertyMap here
   if (m_parent->contains("glColor"))
      spectrum = m_parent->getSpectralSampleSet("glColor", m_pt);
   else if (m_parent->contains("kd"))
      spectrum = m_parent->getSpectralSampleSet("kd", m_pt);
   else 
      spectrum = SpectralSampleSet(0.8, 0.8, 0.8);
   
   if (spectrum == SpectralSampleSet::black())
      spectrum = SpectralSampleSet(0.8, 0.8, 0.8);
   
   glColor3real_tv(spectrum.getRGB().data);
}

}

