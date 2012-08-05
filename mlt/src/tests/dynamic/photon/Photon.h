/**<!-------------------------------------------------------------------->
   @class  Photon
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008
   
   @brief
      Small wrapper for 'photon' data used internally during photon mapping
   
   @see PhotonMap
   @see PhotonMapper
   @see Jensen's book, "Realistic Image Synthesis Using Photon Mapping" (2001)
      for further details
   <!-------------------------------------------------------------------->**/

#ifndef PHOTON_H_
#define PHOTON_H_

#include <utils/SpectralSampleSet.h>
#include <common/math/algebra.h>

struct Photon : public SSEAligned {
   /**
    * position of photon in scene (decoupled from surface at this point)
    */
   Point3   position;
   
   /**
    * if 'wi' represents incident direction of this photon at the time it 
    * hit a surface, 'w' represents -wi, the reversed direction.
    * the reason we store the reversed direction here instead of wi is 
    * because during photon map irradiance estimates, we will always be 
    * interested in -wi, the vector pointed away from the surface's normal
    * in order to evaluate the BSDF at that point (note, this assumes 
    * the BSDFs in question are symmetric, though it's not particularly 
    * important to understand why; all physically real BSDFs are symmetric, 
    * so this assumption doesn't add any bias)
    */
   Vector3  w;
   
   /**
    * incident flux carried by this photon traveling in direction -w
    */
   SpectralSampleSet Li;
   
   /**
    * used internally by PhotonKdTree for internal nodes to store the index of a
    * child photon representing the location of the current node's split plane
    */
   short    plane;
   
   /**
    * convenience constructor
    */
   inline Photon(const Point3   &position_, const Vector3 &w_, 
                 const SpectralSampleSet &Li_)
      : position(position_), w(w_), Li(Li_)
   { }
   
   inline Photon()
   { }
};

#endif // PHOTON_H_

