/**<!-------------------------------------------------------------------->
   @file   PathVertex.h
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

#include <PathVertex.h>

namespace milton {

PathVertex::PathVertex(SurfacePoint *pt_, real_t pL_, real_t pE_)
   : pt(SurfacePointPtr(pt_)), bsdf(pt->bsdf), GL(1), GE(1), 
     alphaL(SpectralSampleSet::identity() / pL_), 
     alphaE(SpectralSampleSet::identity() / pE_), 
     pdfL(1), pdfE(1), pL(pL_), pE(pE_)
{
   ASSERT(pt_);
   
   if (pt->emitter->isEmitter()) {
      alphaL = pt->emitter->getLe0() / pL;
   } else if (pt->sensor->isSensor()) {
      alphaE = pt->sensor->getWe0()  / pE;
   }
   
   wi = -pt->normalS;
   
   if (pt->emitter->isEmitter()) {
      bsdf = pt->emitter;
   } else if (pt->sensor->isSensor()) {
      bsdf = pt->sensor;
   } else {
      bsdf = pt->bsdf;
   }
   
   _init();
}

void PathVertex::_init() {
   ASSERT(pt.get() != NULL);
   ASSERT(pt.use_count() > 0);
   ASSERT(pt->emitter && pt->sensor);
   ASSERT(bsdf);
   
   bsdf->setWi(wi);
   
   // the same metadata will be used for all future BSDF evaluations
   // at this PathVertex
   event = bsdf->sample();
}

std::ostream &operator<<(std::ostream &os, const PathVertex &v) {
   const bool emitter = v.isEmitter();
   const bool sensor  = v.isSensor();
   
   os << "{ " 
      << (sensor ? "camera" : (emitter ? "light" : "")) 
      << (v.bsdf->isSpecular() ? " (specular)" : "") << endl
      //<< "  pt     = " << (*v.pt.get()) << endl
      << "  shape  = " << (unsigned long) v.pt->shape << endl
      << "  GL     = " << v.GL << endl
      << "  GE     = " << v.GE << endl
      << "  tL     = " << v.tL << endl
      << "  tE     = " << v.tE << endl
      << "  alphaL = " << v.alphaL << endl
      << "  alphaE = " << v.alphaE << endl
      << "  fs     = " << v.fs << endl
      << "  pdfL   = " << v.pdfL << endl
      << "  pdfE   = " << v.pdfE << endl
      << "  pL     = " << v.pL << endl
      << "  pE     = " << v.pE << endl;
   
   if (emitter)
      os << "  Le0    = " << v.pt->emitter->getLe0() << endl;
   
   if (sensor)
      os << "  We0    = " << v.pt->sensor->getWe0() << endl;
   
   os << "}";
   return os;
}

}

