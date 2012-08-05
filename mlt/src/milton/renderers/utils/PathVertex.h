/**<!-------------------------------------------------------------------->
   @class  PathVertex
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      A Path is composed of zero or more PathVertex vertices stored in a 
   PathVertexList.  Each PathVertex stores the underlying SurfacePoint, 
   representing a single point on a surface (and all relevant associated 
   metadata, eg, UV coords, normal, BSDF, etc.).  In addition to storing 
   the underlying SurfacePoint, each PathVertex also stores a set of 
   carefully chosen, local values which facilitate efficient computation
   of the unweighted, bidirectional contributions of all possible 
   combinations of light and eye subpaths.  Specifically, 'alphaL' and 
   'alphaE' store the cumulative light and eye contributions respectively, 
   with respect to this vertex' index within its parent Path.
   
   @note
      Density with respect to projected solid angle is density with respect to 
   ordinary solid angle divided by cos(theta) where theta is the angle between 
   wo and the surface normal at v (PathVertex::pdfL and PathVertex::pdfE are 
   both probability densities with respect to projected solid angle)
   
   @see section 10.2 (pgs 302-305) of Veach's thesis for more details
   <!-------------------------------------------------------------------->**/

#ifndef PATH_VERTEX_H_
#define PATH_VERTEX_H_

#include <utils/SpectralSampleSet.h>
#include <materials/Material.h>
#include <core/SurfacePoint.h>
#include <stats/Event.h>

#include <ostream>
#include <deque>

namespace milton {

struct MILTON_DLL_EXPORT PathVertex : public SSEAligned {
   ///@name Fast-access public data
   //@{-----------------------------------------------------------------
   
   SurfacePointPtr pt;        // PT
   
   // pt->bsdf if normal vertex; pt->emitter if initial vertex on light source
   //                            pt->sensor  if initial vertex on sensor (camera)
   BSDF          *bsdf;       // PT
   
   // G(xi<->xi+1)
   real_t         GL;         // L
   // G(xi-1<->xi)
   real_t         GE;         // E
   
   real_t tL;
   real_t tE;
   
   Vector3        wi;         // PT
   
   // cumulative, unweighted bidirectional contributions of light and eye 
   // subpaths respectively
   SpectralSampleSet alphaL;  // L
   SpectralSampleSet alphaE;  // E
   
   // fs(xi-1,xi,xi+1)
   // BSDF value centered at this vertex which always follows flow of light 
   // because fs in the opposite direction is never needed / evaluated
   // (note that we are not assuming BSDF reciprocity here)
   SpectralSampleSet fs;      // PT
   
   // probability density with respect to projected solid angle of sampling 
   // the following vertex in the direction of lightflow in the parent Path 
   // from this vertex; p(xi-1 -> xi -> xi+1) = p(xi+1 | xi, xi-1)
   real_t         pdfL;       // L
   
   // probability density with respect to projected solid angle of sampling 
   // the previous vertex in the direction of lightflow in the parent Path 
   // from this vertex; p(xi-1 <- xi <- xi+1) = p(xi-1 | xi, xi+1)
   real_t         pdfE;       // E
   
   // cumulative probability density up to and including this vertex in parent path
   real_t         pL;         // L
   real_t         pE;         // E
   
   // event which generated the next vertex in the parent Path from this 
   // vertex; metadata associated with this event is used for future BSDF
   // evaluations, s.t. mixture BSDFs which are, for instance, composed of 
   // specular and diffuse components will remain consistent throughout the 
   // lifetime of this PathVertex
   Event          event;      // PT
   
   
   //@}-----------------------------------------------------------------
   ///@name Constructors
   //@{-----------------------------------------------------------------
   
   /**
    * @brief
    *    Initializes a PathVertex
    * 
    * @note if alphaL_ is specified, vertex is assumed to be sampled starting 
    *    from an emitter
    * @note if alphaE_ is specified, vertex is assumed to be sampled starting 
    *    from a  sensor (camera)
    * @note this means that one of alphaL_ or alphaE_ will be disregarded
    */
   inline PathVertex(SurfacePoint *pt_, const Vector3 &wi_, 
                     real_t GL_, real_t GE_, 
                     const SpectralSampleSet &alphaL_, 
                     const SpectralSampleSet &alphaE_, 
                     real_t pL_ = 1, real_t pE_ = 1)
      : pt(SurfacePointPtr(pt_)), bsdf(pt->bsdf), GL(GL_), GE(GE_), wi(wi_), 
        alphaL(alphaL_), alphaE(alphaE_), pdfL(1), pdfE(1), pL(pL_), pE(pE_)
   {
      _init();
   }
   
   PathVertex(SurfacePoint *pt_, real_t pL_, real_t pE_);
   
   inline PathVertex(const PathVertex &copy)
      : pt(copy.pt), bsdf(copy.bsdf), GL(copy.GL), GE(copy.GE), 
        tL(copy.tL), tE(copy.tE), wi(copy.wi), 
        alphaL(copy.alphaL), alphaE(copy.alphaE), fs(copy.fs), 
        pdfL(copy.pdfL), pdfE(copy.pdfE), pL(copy.pL), pE(copy.pE), 
        event(copy.event)
   { }
   
   inline PathVertex()
   { }
   
   
   //@}-----------------------------------------------------------------
   ///@name Miscellaneous Accessors
   //@{-----------------------------------------------------------------
   
   inline bool isEmitter() const {
      return (bsdf == pt->emitter);
   }
   
   inline bool isSensor() const {
      return (bsdf == pt->sensor);
   }
   
   inline bool operator==(const PathVertex &rhs) const {
      return (pt->position == rhs.pt->position && 
              EQ(pt->uv.u, rhs.pt->uv.u) &&
              EQ(pt->uv.v, rhs.pt->uv.v));
   }
   
   inline bool operator!=(const PathVertex &rhs) const {
      return !(*this == rhs);
   }
   
   
   //@}-----------------------------------------------------------------
   
   private:
      void _init();
};

/// Prints a PathVertex to an output stream
MILTON_DLL_EXPORT std::ostream &operator<<(std::ostream &os, const PathVertex &v);

DECLARE_STL_TYPEDEF(std::deque<PathVertex>, PathVertexList);

}

#endif // PATH_VERTEX_H_

