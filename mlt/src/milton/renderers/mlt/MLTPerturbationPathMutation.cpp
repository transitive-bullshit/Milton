/**<!-------------------------------------------------------------------->
   @file   MLTPerturbationPathMutation.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      MLTPerturbationPathMutation implements lens and caustic perturbations.
   The core idea shared by these mutations is that by slightly moving vertices 
   along a path, you can explore nearby paths that will make similar 
   contributions to the image, thereby increasing the acceptance probability.  
   Perturbations are designed to complement the strengths and weaknesses of 
   bidirectional mutations in that they make relatively small, low-cost 
   changes to paths that have a larger acceptance probability as opposed to 
   bidirectional mutations which are in charge of making large scale changes 
   (making successive paths less correlated) at the expense of generally lower 
   acceptance probabilities.
      This class implements two types of perturbation mutations:
   Lens perturbations concentrate on small changes to the film plane location 
   by modifying the lens edge of the form (L|D)DS*E.  Caustic perturbations 
   also change the lens edge but are designed to concentrate solely on primary 
   caustic paths of the form LS+DE.  Multi-chain perturbations focus on 
   paths with a suffix of the form (D|L)DS+DS+E and are currently not 
   implemented.
   
   @see p. 350-354 of Veach's thesis (section 11.4.3)
   <!-------------------------------------------------------------------->**/

#include "MLTPerturbationPathMutation.h"
#include <RenderOutput.h>
#include <Camera.h>
#include <QtCore/QtCore>
#include <mlt.h>

namespace milton {

std::ostream ERR_TEST(NULL);
//std::ostream &ERR_TEST = std::cerr;

enum PERTURBATION_TYPE {
   PERTURBATION_TYPE_LENS,        // suffix    (L|D)DS*E
   PERTURBATION_TYPE_MULTI_CHAIN, // suffix (L|D)DS+DS*E
   PERTURBATION_TYPE_CAUSTIC,     // suffix    (L|D)S+DE todo?: (L|D)S*DE
   
   // TODO: handle LSDSE
};

void MLTPerturbationPathMutation::init() {
   const Viewport &viewport = m_renderer->getOutput()->getViewport();
   
   // lens perturbation
   m_r1 = 0.1 * viewport.getInvWidth();
   m_r2 = 0.05 / 2.0; // 5% of image size
   
   // caustic perturbation
   //m_t1 = 0.0001; // 0.0001
   //m_t2 = 0.1; // 0.1
   m_t1 = 0.05; // 0.0001
   m_t2 = 0.3; // 0.1
}

real_t MLTPerturbationPathMutation::mutate(const Path &X, Path &Y) {
   const unsigned n = X.length();
   PERTURBATION_TYPE type;
   Path lens(m_renderer);
   Y = X;
   
   { // delete current lens subpath
      Y.pop_back();              // pop previous sensor vertex
      
      if (!Y.back().bsdf->isSpecular() && Y.length() >= 2 && 
          Y[Y.length() - 2].bsdf->isSpecular())
      {                          // path has suffix SDE
         type = PERTURBATION_TYPE_CAUSTIC;
         
         Y.pop_back();           // pop diffuse vertex
         while(Y.back().bsdf->isSpecular()) {
            Y.pop_back();        // pop zero or more specular vertices
            
            if (Y.empty())
               return 0;         // mutation failed
         }
      } else {
         type = PERTURBATION_TYPE_LENS;
         
         while(Y.back().bsdf->isSpecular()) {
            Y.pop_back();        // pop zero or more specular vertices
            
            if (Y.empty())
               return 0;         // mutation failed
         }
         
         Y.pop_back();           // pop diffuse vertex
         
         while(Y.length() >= 2 && Y.back().bsdf->isSpecular()) {
            // path has suffix SDS*E and is a multi-chain candidate
            type = PERTURBATION_TYPE_MULTI_CHAIN;
            
            do {                 // remove one or more specular vertices
               Y.pop_back();
               
               if (Y.empty())
                  return 0;      // mutation failed
            } while(Y.back().bsdf->isSpecular());
            
            Y.pop_back();        // pop diffuse vertex
         }
         
         if (type == PERTURBATION_TYPE_LENS && !Y.empty() && 
             Y.back().bsdf->isSpecular())
         {
            return 0;            // mutation failed
         }
      }
   }
   
   const unsigned s = Y.length();
   
   if (type == PERTURBATION_TYPE_LENS || 
       type == PERTURBATION_TYPE_MULTI_CHAIN)
   {
      { // determine new point on image plane
         const Viewport &viewport = m_renderer->getOutput()->getViewport();
         const real_t pA = viewport.getInvWidth() * viewport.getInvHeight();
         Point2 filmPt(X.back().pt->uv.u, X.back().pt->uv.v);
         
         /*const real_t theta = Random::sample(0, 2 * M_PI);
         const real_t r     = m_r2 * exp(-log(m_r2 / m_r1) * Random::sample(0, 1));
         
         filmPt[0] += r * cos(theta);
         filmPt[1] += r * sin(theta);*/
         
         filmPt[0] += Random::sample(-5, 5) * viewport.getInvWidth ();
         filmPt[1] += Random::sample(-5, 5) * viewport.getInvHeight();
         
         filmPt[0]  = CLAMP(filmPt[0], 0, 1);
         filmPt[1]  = CLAMP(filmPt[1], 0, 1);
         
         SurfacePoint *pt = new SurfacePoint();
         m_renderer->getCamera()->getPoint(*pt, UV(filmPt));
         if (!lens.prepend(PathVertex(pt, 1, pA)))
            return 0;            // mutation failed
      }
      
      // prepend vertices until we reach a non-specular vertex
      do {
         if (!lens.prepend(false, false))
            return 0;            // mutation failed
         
         if (lens.length() > n)
            return 0;            // mutation failed
         
         if (lens.front().pt->shape->getMaterial() != 
             X[n - lens.length()].pt->shape->getMaterial())
         {
            return 0;            // mutation failed
         }
         
         lens.front().bsdf->setWi(lens.front().wi);
         lens.front().event = 
            lens.front().bsdf->sample(X[n - lens.length()].event);
      } while(lens.front().bsdf->isSpecular());
      
      if (type == PERTURBATION_TYPE_MULTI_CHAIN) {
         do {                    // add one or more DS+ chains
            const unsigned t = n - lens.length();
            if (t == 0 || Y.length() + lens.length() >= n)
               break;
            
//#warning ""
//#warning ""
// WARNING:  "trying multi-chain not perturbing interior vertices"  (current; commented out for windows)
//#warning ""
//#warning ""
            
            Vector3 wo = (X[t - 1].pt->position - X[t].pt->position).getNormalized();
            _perturbVector(wo);
            
            PathVertex &v = lens.front();
            v.event = Event(wo, v.event);
            
            do {                 // add DS+
               if (!lens.prepend(false, false))
                  return 0;     // mutation failed
               
               if (lens.length() > n)
                  return 0;      // mutation failed
               
               if (lens.front().pt->shape->getMaterial() != 
                   X[n - lens.length()].pt->shape->getMaterial())
               {
                  return 0;     // mutation failed
               }
               
               lens.front().bsdf->setWi(lens.front().wi);
               lens.front().event = 
                  lens.front().bsdf->sample(X[n - lens.length()].event);
            } while(lens.front().bsdf->isSpecular());
         } while(lens.length() + Y.length() < n);
      }
   } else {
      ASSERT(type == PERTURBATION_TYPE_CAUSTIC);
      ASSERT(X[s - 1].pt->position == Y.back().pt->position);
      ASSERT(X[s].bsdf->isSpecular());
      ASSERT(!Y.back().bsdf->isSpecular());
      
      /*real_t t = (X[n - 1].pt->position - X[n - 2].pt->position).getNormalized();
      for(unsigned i = s; i < n - 1; ++i)
      t /= (X[i].pt->position - X[i - 1].pt->position).getNormalized();*/
      
      Vector3 wo = (X[s].pt->position - X[s - 1].pt->position).getNormalized();
      _perturbVector(wo);
      
      PathVertex &v = Y.back();
      v.event = Event(wo, v.event);
      
      do {
         if (!Y.append(false, false))
            return 0;            // mutation failed
         
         if (Y.back().pt->shape->getMaterial() != X[Y.length() - 1].pt->shape->getMaterial())
            return 0;            // mutation failed
         
         if (Y.length() >= n - 1)
            break;
         
         Y.back().bsdf->setWi(Y.back().wi);
         Y.back().event = Y.back().bsdf->sample(X[Y.length() - 1].event);
      } while(1);
      
      { // determine new point on image plane
         const Viewport &viewport = m_renderer->getOutput()->getViewport();
         const real_t pA = viewport.getInvWidth() * viewport.getInvHeight();
         Point2 filmPt = m_renderer->getCamera()->getProjection(Y.back().pt->position);
         
         filmPt[0] = CLAMP(filmPt[0], 0, 1);
         filmPt[1] = CLAMP(filmPt[1], 0, 1);
         
         Point2 orig(X.back().pt->uv.u, X.back().pt->uv.v);
         //cerr << filmPt << " vs " << orig << endl;
         
         SurfacePoint *pt = new SurfacePoint();
         m_renderer->getCamera()->getPoint(*pt, UV(filmPt));
         if (!lens.prepend(PathVertex(pt, 1, pA)))
            return 0;            // mutation failed
      }
   }
   
   { // attempt to join old subpath with new lens subpath
      if (!Y.append(lens))
         return 0;               // mutation failed
      
      if (!Y.front().isEmitter() || !Y.back().isSensor())
         return 0;               // mutation failed
   }
   
   real_t Txy = 1, Tyx = 1;
   
   switch(type) {
      case PERTURBATION_TYPE_CAUSTIC:
         Txy = Y[Y.length() - 1].pL;
         Tyx = X[X.length() - 1].pL;
         break;
      case PERTURBATION_TYPE_LENS:
      case PERTURBATION_TYPE_MULTI_CHAIN:
      default:
         Txy = Y[s].pE;
         Tyx = X[s].pE;
         break;
   }
   
   const real_t Fx    = X.getRadiance().getRGB().luminance();
   const real_t den   = Txy * Fx;
   
   if (den == 0)
      return 0;                  // mutation failed
   
// TODO:  #warning "TESTING"
   const real_t a = ((Y.getRadiance()/X.getRadiance()).getRGB()).luminance() * (Tyx/Txy);
   return MIN(1.0, a);
   
   const real_t Fy    = Y.getRadiance().getRGB().luminance();
   const real_t alpha = MIN(1.0, (Fy * Tyx) / den);
   
   /*if (type == PERTURBATION_TYPE_MULTI_CHAIN)
      cerr << "multi-chain " << "alpha: " << alpha << endl;
   else if (type == PERTURBATION_TYPE_LENS)
      cerr << "lens        " << "alpha: " << alpha << endl;
   else 
      cerr << "caustic     " << "alpha: " << alpha << endl;*/
   
   return alpha;
}

#if 0
real_t MLTPerturbationPathMutation::getPd(const Event &event) {
   Path Y = event.getValue<const Path&>();
   Path y = Y;
   Path x = m_path;
   
   if (x.length() < 2 || y.length() < 2)
      return 0;
   
   { // delete current lens subpaths
      ASSERT(x.front().isEmitter());
      ASSERT(x.back() .isSensor());
      ASSERT(y.front().isEmitter());
      ASSERT(y.back() .isSensor());
      
      do {
         x.pop_back();
      } while(x.back().bsdf->isSpecular());
      
      do {
         y.pop_back();
      } while(y.back().bsdf->isSpecular());
      
      if (y.empty() || x.empty())
         return 0; // invalid mutation
      
      y.pop_back();
      x.pop_back();
   }
   
   // ensure non-lens subpaths match up
   if (x != y)
      return 0;    // invalid mutation
   
   const unsigned s = y.length();
   
#if 1
   const real_t luminance = Y.getRadiance().getRGB().luminance();
   const real_t p = Y[s].pE;
   
   if (luminance <= 0)
      return 0;
   
   return p / luminance;
#else
   const unsigned t = Y.length() - s;
   const SpectralSampleSet &radiance = Y.getContribution(s, t);
   const real_t luminance = radiance.getRGB().luminance();
   
   if (luminance == 0)
      return 0;
   
   return 1.0 / luminance;
#endif
}
#endif

void MLTPerturbationPathMutation::_perturbVector(Vector3 &N) const {
   Vector3 U, V;
   N.getOrthonormalBasis(U, V);
   
   const real_t phi = Random::sample(0, 2 * M_PI);
   const real_t r   = m_t2 * exp(-log(m_t2 / m_t1) * Random::sample(0, 1));
   
   N = N + r * cos(phi) * U + r * sin(phi) * V;
   (void) N.normalize();
}

}

