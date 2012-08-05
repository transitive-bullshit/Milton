/**<!-------------------------------------------------------------------->
   @file   Path.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Core data structure for manipulating a sequence x0,x1,...,xk of points on 
   scene surfaces. Paths are the central unit in the path integral formulation
   of light transport, upon which path tracing, bidirectional path tracing, and
   MLT are all founded.
   
   Each Path is assumed to either start at an emitter (light subpath), end at a 
   sensor (eye subpath), or both start at an emitter and end at a sensor 
   (complete, valid path).
   
   @note individual vertices (see PathVertex) are stored internally in an 
      std::deque
   <!-------------------------------------------------------------------->**/

#include <Path.h>
#include <PointSampleRenderer.h>
#include <RenderOutput.h>
#include <Renderer.h>
#include <ShapeSet.h>
#include <Camera.h>
#include <Scene.h>
#include <QtCore/QtCore>
#include <typeinfo>

namespace milton {

// comment or uncomment the next line to enable or disable a hacky fix for 
// extremely large geometry terms caused by surface points being very close 
// to each other -- leading to numerical instabilities and infamous "bright 
// spots"
//#define GEOMETRY_HACK (1)

// comment or uncomment the next line to enable or disable invalidating 
// really small distances between consequtive vertices in a path
#define GEOMETRY_BOUND (EPSILON)
//#define GEOMETRY_BOUND (0.04 * m_renderer->getScene()->getShapes()->getAABB().getDiagonal().getMagnitude())

// temporarily testing
#define USE_FILM_PLANE_PROBABILITY (1)


SpectralSampleSet Path::getContribution(unsigned s, unsigned t, bool tentative) {
   // note s and t are exclusive; u and v are inclusive
   const unsigned n = length();
   const unsigned u = s - 1; // index of last light subpath vertex
   const unsigned v = n - t; // index of last eye   subpath vertex
   ASSERT(s + t <= n && n >= 2);
   
   if (0 == s) {            // empty light subpath
      // point lights cannot contribute to 0-length light paths because it is 
      // impossible for a point light to be intersected by random eye paths
      if (m_vertices[v].pt->shape->getSurfaceArea() < EPSILON)
         return SpectralSampleSet::black(); // point light
      
      Vector3 wo = -m_vertices[v].wi;
      
      if (v < n - 1) {
         wo = (m_vertices[v + 1].pt->position - 
               m_vertices[v].pt->position).getNormalized();
      }
      
      return (
         m_vertices[v].pt->emitter->getLe(wo) * 
         m_vertices[v].alphaE
      );
   } else if (0 == t) {     // empty eye subpath
      // pinhole camera cannot contribute to 0-length eye paths because it is 
      // impossible for the pinhole to be intersected by random light paths
      if (m_vertices[u].pt->shape->getSurfaceArea() < EPSILON)
         return SpectralSampleSet::black(); // pinhole camera
      
      Vector3 wo = -m_vertices[u].wi;
      
      if (u > 0) {
         wo = (m_vertices[u - 1].pt->position - 
               m_vertices[u].pt->position).getNormalized();
      }
      
      return (
         m_vertices[u].alphaL * 
         m_vertices[u].pt->sensor->getWe(wo)
      );
   } else {
      ASSERT(u < v);
      ASSERT(v < n);
      PathVertex &y = m_vertices[u];
      PathVertex &z = m_vertices[v];
      
      // if either connecting vertex is specular, the contribution would be zero
      // with probability one (since a specular vertex' BSDF is non-zero over 
      // a set of directions with measure zero). we define the contribution of 
      // such connecting paths to be zero, thereby disregarding their 
      // theoretically zero chance of actually making a contribution
      if (y.bsdf->isSpecular() || z.bsdf->isSpecular())
         return SpectralSampleSet::black();
      
      if (s + t == n) {     // special case for full-length paths, where
                            // visibility is implicitly true and fs and G 
                            // are already known
         return (
            y.alphaL * 
            y.fs * y.GL * z.fs * 
            z.alphaE
         );
      } else {              // general case (visibility check needed)
         Vector3 wo = (z.pt->position - y.pt->position);
         real_t t   = wo.normalize();
         
#ifdef GEOMETRY_BOUND
         if (t < GEOMETRY_BOUND)
            return SpectralSampleSet::black();
#endif
         
         // NOTE: used to be commented out for tentative
         if (!tentative &&  // ensure visibility between y and z
             m_renderer->getScene()->intersects(Ray(y.pt->position, wo), t))
         {
            return SpectralSampleSet::black();
         }
         
         if (u > 0) {
            y.bsdf->setWi(
               (y.pt->position - m_vertices[u - 1].pt->position).getNormalized());
         }
         
         Vector3 wo2 = -wo;
         if (v < n - 1)
            wo2 = (m_vertices[v + 1].pt->position - z.pt->position).getNormalized();
         
         z.bsdf->setWi(wo);
         
#ifdef GEOMETRY_HACK
         t += 1;
#endif
         
         real_t G = ABS(y.pt->normal.dot(wo) * z.pt->normal.dot(-wo)) / (t * t);
         
         return (
            y.alphaL * 
            y.bsdf->evaluate(wo) * G * z.bsdf->evaluate(wo2) * 
            z.alphaE
         );
      }
   }
}

real_t Path::getPd(unsigned s, unsigned t, bool tentative) {
   // note s and t are exclusive; u and v are inclusive
   const unsigned n = length();
   const unsigned u = s - 1; // index of last light subpath vertex
   const unsigned v = n - t; // index of last eye   subpath vertex
   ASSERT(s + t <= n && n >= 2);
   
   if (0 == s) {            // empty light subpath
      // point lights cannot contribute to 0-length light paths because it is 
      // impossible for a point light to be intersected by random eye paths
      if (m_vertices[v].pt->shape->getSurfaceArea() < EPSILON)
         return 0;
      
      return m_vertices[v].pE;
   } else if (0 == t) {     // empty eye subpath
      // pinhole camera cannot contribute to 0-length eye paths because it is 
      // impossible for the pinhole to be intersected by random light paths
      if (m_vertices[u].pt->shape->getSurfaceArea() < EPSILON)
         return 0;
      
      return m_vertices[u].pL;
   } else {
      ASSERT(u < v);
      PathVertex &y = m_vertices[u];
      PathVertex &z = m_vertices[v];
      
      // if either connecting vertex is specular, the contribution would be zero
      // with probability one (since a specular vertex' BSDF is non-zero over 
      // a set of directions with measure zero). we define the contribution of 
      // such connecting paths to be zero, thereby disregarding their 
      // theoretically zero chance of actually making a contribution
      if (y.bsdf->isSpecular() || z.bsdf->isSpecular())
         return 0;
      
      if (s + t == n) {     // special case for full-length paths, where
                            // visibility is implicitly true and fs and G 
                            // are already known
         return y.pL * z.pE;
      } else {              // general case (visibility check needed)
         Vector3 wo = (z.pt->position - y.pt->position);
         real_t t   = wo.normalize();
         
#ifdef GEOMETRY_BOUND
         if (t < GEOMETRY_BOUND)
            return 0;
#endif
         
         if (!tentative && // ensure visibility between y and z
             m_renderer->getScene()->intersects(Ray(y.pt->position, wo), t))
            return 0;
         
         return y.pL * z.pE;
      }
   }
}

void Path::_computeRadiance() {
   const unsigned n = length();
   
   if (n < 2) {
      m_radiance = SpectralSampleSet::black();
      return;
   }
   
   m_radiance = m_vertices[0].pt->emitter->getLe0();
   
   //cerr << *this << endl << endl;
   for(unsigned i = 0; i < n; ++i) {
      m_radiance *= m_vertices[i].fs * m_vertices[i].GL;
         /*(i >= n - 1 || m_vertices[i].bsdf->isSpecular() ? 1 : 
          m_vertices[i].pt->normal.dot(
             (m_vertices[i + 1].pt->position - m_vertices[i].pt->position).getNormalized()));*/
   }
}

// add a new vertex to a Light subpath
bool Path::append(bool roulette, bool sampleBSDF) {
   if (length() > 0) // general case
      return _samplePathVertex(roulette, false, sampleBSDF);
   
   // sample initial location on random light source
   EmitterSampler emitterSampler = m_renderer->getScene()->getEmitterSampler();
   Event event = emitterSampler.sample();
   SurfacePoint *pt = event.getValue<SurfacePoint*>();
   const real_t pA  = emitterSampler.getPd(event);
   
   m_vertices.push_back(PathVertex(pt, pA, 1));
   back().tE = 0;
   
   return true;
}

// add a new vertex to an Eye subpath
bool Path::prepend(bool roulette, bool sampleBSDF) {
   if (length() > 0) // general case
      return _samplePathVertex(roulette, true, sampleBSDF);
   
   // sample initial location on sensor
   Camera *camera   = m_renderer->getCamera();
   SurfacePoint *pt = new SurfacePoint();
   real_t pA = 1.0;
   
#ifdef USE_FILM_PLANE_PROBABILITY
   PointSampleRenderer *renderer = NULL;
   
   try {
      renderer = dynamic_cast<PointSampleRenderer*>(m_renderer);
   } catch(std::bad_cast&) { }
   
   if (renderer) {
      const Viewport &viewport = renderer->getOutput()->getViewport();
      pA = viewport.getInvWidth() * viewport.getInvHeight();
   }
#endif
   
   camera->getRandomPoint(*pt);
   m_vertices.push_front(PathVertex(pt, 1, pA));
   front().tL = 0;
   
   return true;
}

// add a new vertex to an eye subpath
bool Path::prepend(const PathVertex &v1) {
   if (empty()) {
      m_vertices.push_front(v1);
      front().tL = 0;
      ASSERT(length() == 1);
   } else {
      PathVertex &v = front();
      SpectralSampleSet alphaE;
      real_t pE = 0;
      
      Vector3 wo = (v1.pt->position - v.pt->position);
      real_t   t = wo.normalize();
      
      Vector3 wi = -wo;
      if (length() >= 2)
         wi = (v.pt->position - m_vertices[1].pt->position).getNormalized();
      
      if (!_initE(v, wi, wo, t, v1.pt.get(), alphaE, pE, false))
         return false;
      
      m_vertices.push_front(v1);
      PathVertex &z = m_vertices.front();
      z.GL = v.GE;
      z.wi = wo;
      z.alphaL = SpectralSampleSet::identity();
      z.alphaE = alphaE;
      z.pL = 1;
      z.pE = pE;
      z.tL = t;
      
      if (z.bsdf == z.pt->sensor)
         z.bsdf = z.pt->bsdf;
   }
   
   return true;
}

// add a new vertex to a light subpath
bool Path::append(const PathVertex &v1) {
   if (empty()) {
      m_vertices.push_back(v1);
      back().tE = 0;
      ASSERT(length() == 1);
   } else {
      PathVertex &v = back();
      SpectralSampleSet alphaL;
      real_t pL = 0;
      
      Vector3 wo = (v1.pt->position - v.pt->position);
      real_t   t = wo.normalize();
      
      Vector3 wi = -wo;
      if (length() >= 2)
         wi = (v.pt->position - m_vertices[length() - 2].pt->position).getNormalized();
      
      if (!_initL(v, wi, wo, t, v1.pt.get(), alphaL, pL, false))
         return false;
      
      m_vertices.push_back(v1);
      PathVertex &y = m_vertices.back();
      y.GE = v.GL;
      y.wi = wo;
      y.alphaL = alphaL;
      y.alphaE = SpectralSampleSet::identity();
      y.pL = pL;
      y.pE = 1;
      y.tE = t;
      
      if (y.bsdf == y.pt->emitter)
         y.bsdf = y.pt->bsdf;
   }
   
   return true;
}

bool Path::append(const Path &path) {
   const unsigned s = length();
   const unsigned t = path.length();
   const unsigned k = s + t;
   unsigned invalid = 0;
   
   // append the underlying PathVertexLists together
   m_vertices.insert(m_vertices.begin() + s, 
                     path.m_vertices.begin(), path.m_vertices.end());
   ASSERT(k == length());
   
   if (k > 0 && m_vertices[0].pt->emitter->isEmitter())
      m_vertices[0].bsdf = m_vertices[0].pt->emitter;
   
   { // initialize connection between last light subpath vertex and first 
     // eye subpath vertex
     if (s > 0 && t > 0) {
         PathVertex &y = m_vertices[s - 1];
         PathVertex &z = m_vertices[s];
         
         Vector3 wo = (z.pt->position - y.pt->position);
         real_t  d  = wo.normalize();
         
         SpectralSampleSet alpha; // unused
         real_t pA; // unused
         
         Vector3 wi = -wo;
         if (s > 1)
            wi = (y.pt->position - m_vertices[s - 2].pt->position).getNormalized();
         
         if (!_initL(y, wi,  wo, d, z.pt.get(), alpha, pA, false))
            invalid |= 3; // 1
         
         wi = wo;
         if (t > 1)
            wi = (z.pt->position - m_vertices[s + 1].pt->position).getNormalized();
         
         if (!_initE(z, wi, -wo, d, y.pt.get(), alpha, pA, false))
            invalid |= 3; // 2
         
         // ensure visibility between y and z
         if (invalid != 3 && m_renderer->getScene()->intersects(Ray(y.pt->position, wo), d)) {
            y.GL = z.GE = 0;
            
            invalid |= 3;
         }
      }
   }
   
   { // merge cumulative eye and light subpath contributions
      // update cumulative eye contribution in original light subpath
      for(unsigned i = s; i--;) {
         PathVertex &y = m_vertices[i];
         
         if (i >= k - 1) { // last light vertex connected to empty eye subpath
            ASSERT(0 == t);
            ASSERT(!invalid);
            
            if (y.pt->sensor->isSensor()) {
               real_t pA = m_renderer->getCamera()->getSurfaceArea();
               pA = (pA > EPSILON ? 1.0 / pA : 1);
               
#ifdef USE_FILM_PLANE_PROBABILITY
               PointSampleRenderer *renderer = NULL;
               
               try {
                  renderer = dynamic_cast<PointSampleRenderer*>(m_renderer);
               } catch(std::bad_cast&) { }
               
               if (renderer) {
                  const Viewport &viewport = renderer->getOutput()->getViewport();
                  pA *= viewport.getInvWidth() * viewport.getInvHeight();
               }
#endif
               
               //ASSERT(0 && "shouldn't get here with pinhole camera!!!");
               // TODO: totally unsure about the folling 10~ish lines
               
               if (y.bsdf != y.pt->sensor) {
                  y.bsdf  = y.pt->sensor;
                  y.event = y.bsdf->sample();
               }
               
               Vector3 wi = y.wi;
               
               if (i > 0)
                  wi = (y.pt->position - m_vertices[i - 1].pt->position).getNormalized();
               
               y.bsdf->setWi(wi);
               
               y.alphaE = y.pt->sensor->getWe0() / pA;
               y.pE     = pA;
               
               ASSERT(y.bsdf == y.pt->sensor);
               y.fs     = y.bsdf->evaluate(-wi);
               y.pdfE   = y.bsdf->getPd(Event(-wi, y.event));
               
               ASSERT(y.bsdf == y.pt->sensor);
            } else {
               y.alphaE = SpectralSampleSet::black();
               y.pE     = 0;
               
               y.fs     = SpectralSampleSet::black();
               y.pdfE   = 0;
            }
            
            y.tL = 0;
         } else {
            const PathVertex &z = m_vertices[i + 1];
            
            //y.tL = z.tE = (z.pt->position - y.pt->position).getMagnitude();
            
            y.alphaE = (z.pdfE > 0 && z.GE > 0 ? 
                        z.alphaE * z.fs / z.pdfE : 
                        SpectralSampleSet::black());
            y.pE     = z.pE * z.pdfE * z.GE;
         }
      }
      
      // update cumulative light contribution in orignal eye subpath
      for(unsigned i = 0; i < t; ++i) {
         PathVertex &z = m_vertices[s + i];
         
         if (0 == s + i) { // last eye vertex connected to empty light subpath
            ASSERT(0 == s);
            ASSERT(!invalid);
            
            if (z.pt->emitter->isEmitter()) {
               const Event event(z.pt.get());
               const real_t pA = 
                  m_renderer->getScene()->getEmitterSampler().getPd(event);
               
               // shouldn't have to setWi for z's BSDF cause it's an emitter
               Vector3 wo = -z.wi;
               if (s + i < k - 1)
                  wo = (m_vertices[s + i + 1].pt->position - z.pt->position).getNormalized();
               
               if (z.bsdf != z.pt->emitter) {
                  z.bsdf  = z.pt->emitter;
                  z.event = z.bsdf->sample();
               }
               
               z.alphaL = z.pt->emitter->getLe0() / pA;
               z.pL     = pA;
               
               z.fs     = z.bsdf->evaluate(wo);
               z.pdfL   = z.bsdf->getPd(Event(wo, z.event));
               
               ASSERT(z.bsdf == z.pt->emitter);
            } else {
               z.alphaL = SpectralSampleSet::black();
               z.pL     = 0;
               
               z.fs     = SpectralSampleSet::black();
               z.pdfL   = 0;
            }
            
            z.tE = 0;
         } else {
            const PathVertex &y = m_vertices[s + i - 1];
            
            //z.tE = y.tL = (y.pt->position - z.pt->position).getMagnitude();
            
            z.alphaL = (y.pdfL > 0 && y.GL > 0 ? 
                        y.alphaL * y.fs / y.pdfL : 
                        SpectralSampleSet::black());
            z.pL     = y.pL * y.pdfL * y.GL;
         }
      }
   }
   
   _computeRadiance();
   return (invalid != 3);
}

bool Path::_samplePathVertex(bool roulette, bool adjoint, bool sampleBSDF) {
   PathVertex &v = (adjoint ? front() : back());
   SurfacePoint *pt;
   Vector3 wi;
   real_t t;
   
   if (length() > 1) {
      if (adjoint)
         wi = (v.pt->position - m_vertices[1].pt->position).getNormalized();
      else 
         wi = (v.pt->position - m_vertices[length() - 2].pt->position).getNormalized();
      
      v.bsdf->setWi(wi);
   }
   
   if (sampleBSDF)
      v.event = v.bsdf->sample();
   
   // sample direction from endpoint's BSDF
   const Vector3 &wo = v.event;
   if (wo == Vector3::zero())
      return false; // absorbed
   
   ASSERT(wo.isUnit());
   { // trace ray to determine new surfacepoint
      const Ray ray(v.pt->position, wo);
      
      pt = new SurfacePoint();
      t  = m_renderer->getScene()->getIntersection(ray, *pt);
      
#ifdef GEOMETRY_BOUND
      if (t < GEOMETRY_BOUND) {
         safeDelete(pt);
         return false;
      }
#endif
      
      if (!pt->init(ray, t)) {
         safeDelete(pt);
         return false;
      }
   }
   
   if (length() == 1)
      wi = -wo;
   
   v.wi = wi;
   
   // initialize new point
   if (adjoint) { // eye subpath
      SpectralSampleSet alphaE;
      real_t pE = 0;
      
      if (!_initE(v, wi, wo, t, pt, alphaE, pE, roulette)) {
         safeDelete(pt);
         return false;
      }
      
      m_vertices.push_front(
         PathVertex(pt, wo, v.GE, 1, SpectralSampleSet::identity(), alphaE, 1, pE)
      );
      
      PathVertex &z = m_vertices.front();
      //if (z.bsdf == z.pt->sensor)
      //   z.bsdf = z.pt->bsdf;
      
      z.tL = t;
   } else {       // light subpath
      SpectralSampleSet alphaL;
      real_t pL = 0;
      
      if (!_initL(v, wi, wo, t, pt, alphaL, pL, roulette)) {
         safeDelete(pt);
         return false;
      }
      
      m_vertices.push_back (
         PathVertex(pt, wo, 1, v.GL, alphaL, SpectralSampleSet::identity(), pL, 1)
      );
      
      PathVertex &y = m_vertices.back();
      //if (y.bsdf == y.pt->emitter)
      //   y.bsdf = y.pt->bsdf;
      
      y.tE = t;
   }
   
   return true;
}

bool Path::_initL(PathVertex &y, const Vector3 &wi, const Vector3 &wo, real_t t,
                  const SurfacePoint *pt, SpectralSampleSet &alphaL, real_t &pL,
                  bool roulette) const
{
#ifdef GEOMETRY_BOUND
   if (t < GEOMETRY_BOUND) {
      y.GL = 0;
      return false;
   }
#endif
   
#ifdef GEOMETRY_HACK
   t += 1;
#endif
   
   y.tL   = t;
   y.GL   = ABS(y.pt->normal.dot(wo) * pt->normal.dot(-wo)) / (t * t);
   
   y.bsdf->setWi(-wo);
   y.pdfE = y.bsdf->getPd(Event(-wi, y.event));
   
   y.bsdf->setWi(wi);
   y.fs   = y.bsdf->evaluate(wo);
   y.pdfL = y.bsdf->getPd(Event(wo, y.event));
   
   if (y.GL == 0 || y.pdfL == 0) {
      pL     = 0;
      alphaL = SpectralSampleSet::black();
      
      return false;
   }
   
   // russian roulette
   if (roulette || y.fs == SpectralSampleSet::black()) {
      const real_t q = MIN(0.95, y.fs[y.fs.getMaxSample()].value / y.pdfL);
      
      if (q <= Random::sample(0, 1)) {
         y.pdfL = 0;
         pL     = 0;
         alphaL = SpectralSampleSet::black();
         
         return false;
      }
      
      y.pdfL *= q;
   }
   
   // accumulate light subpath probability density and contribution
   pL     = (y.pdfL * y.GL) * y.pL;
   alphaL = (y.fs / y.pdfL) * y.alphaL;
   
   return true;
}

bool Path::_initE(PathVertex &z, const Vector3 &wi, const Vector3 &wo, real_t t,
                  const SurfacePoint *pt, SpectralSampleSet &alphaE, real_t &pE,
                  bool roulette) const
{
#ifdef GEOMETRY_BOUND
   if (t < GEOMETRY_BOUND) {
      z.GE = 0;
      return false;
   }
#endif
   
#ifdef GEOMETRY_HACK
   t += 1;
#endif
   
   z.tE   = t;
   z.GE   = ABS(z.pt->normal.dot(wo) * pt->normal.dot(-wo)) / (t * t);
   
   z.bsdf->setWi(-wo);
   z.pdfL = z.bsdf->getPd(Event(-wi, z.event));
   z.fs   = z.bsdf->evaluate(-wi);
   
   z.bsdf->setWi(wi);
   z.pdfE = z.bsdf->getPd(Event(wo, z.event));
   
   if (z.GE == 0 || z.pdfE == 0) {
      pE     = 0;
      alphaE = SpectralSampleSet::black();
      
      return false;
   }
   
   // russian roulette
   if (roulette || z.fs == SpectralSampleSet::black()) {
      const real_t q = MIN(0.95, z.fs[z.fs.getMaxSample()].value / z.pdfE);
      
      if (q <= Random::sample(0, 1)) {
         z.pdfE = 0;
         pE     = 0;
         alphaE = SpectralSampleSet::black();
         
         return false;
      }
      
      z.pdfE *= q;
   }
   
   // accumulate eye subpath probability density and contribution
   pE     = (z.pdfE * z.GE) * z.pE;
   alphaE = (z.fs / z.pdfE) * z.alphaE;
   
   return true;
}

std::string Path::toHeckbertNotation() const {
   std::string str = "";
   
   FOREACH(PathVertexListConstIter, m_vertices, iter) {
      if (iter->isEmitter() && iter->pt->emitter->isEmitter())
         str += "L";
      else if (iter->isSensor() && iter->pt->sensor->isSensor())
         str += "E";
      else 
         str += (iter->pt->bsdf->isSpecular() ? "S" : "D");
   }
   
   return str;
}

std::ostream &operator<<(std::ostream &os, const Path &path) {
   const unsigned k = path.length();
   
#if 0
   for(unsigned i = 0; i < k; ++i) {
      PathVertex &v = const_cast<PathVertex&>(path[i]);
      
      if (i > 0) {
         real_t t = (path[i - 1].pt->position - v.pt->position).getMagnitude();
         
         if (NEQ(v.tE, t))
            cerr << "0) error: " << v.tE << " vs " << t << " vs " << (v.pt->position - path[i - 1].pt->position).getMagnitude() << endl;
         //m_vertices[i - 1].tL = (v.pt->position - m_vertices[i - 1].pt->position).getMagnitude();
         
         //if (NEQ(v.tE , m_vertices[i - 1].tL))
         //   cerr << v.tE << ", " << m_vertices[i - 1].tL << endl;
      } else {
         v.tE = 0;
      }
      if (i < k - 1) {
         real_t t = (path[i + 1].pt->position - v.pt->position).getMagnitude();
         
         if (NEQ(v.tL, t))
            cerr << "1) error: " << v.tE << " vs " << t << " vs " << (v.pt->position - path[i + 1].pt->position).getMagnitude() << endl;
      } else {
         v.tL = 0;
      }
   }
#endif
   
   os << "{ length = " << k;
   
   // print out each vertex in the given path
   for(unsigned i = 0; i < k; ++i)
      os << ", " << path[i];
   
   os << " }";
   
   return os;
}

bool Path::operator==(const Path &p) const {
   unsigned n = length();
   
   if (n != p.length())
       return false;
   
   while(n--) {
      if (m_vertices[n] != p[n])
         return false;
   }
   
   return true;
}


static inline unsigned P_INDEX(unsigned i, bool adjoint, 
                               unsigned n, unsigned k)
{
   ASSERT(i < k);
   
   // index of ith vertex in subpath of length k of original path of length n, 
   // with the ith vertex chosen from the eye if adjoint or from the light if 
   // adjoitn is false
   return (adjoint ? (n - k + i) : i);
}

// note: computing relative pdfs of each of the k + 1 possible ways to generate
// a path with k vertices allows us to disregard the connecting edge and its 
// associated pdf by assuming it is 1 and computing everything else relative to 
// that.
void Path::getPds(unsigned k, unsigned s, real_t *pdfs) {
   const unsigned n = length();
   ASSERT(s <= n && k <= n);
   
   unsigned t = n - s;
   s = MIN(k - t, k);
   // TODO: what to set pdf[s] = 1??
   // TODO: with scene test.js (simple renderWarmup scene), get basically black if 
   // we don't start with pdf = 1 from eye's point-of-view -- incorrect!
   
   /*for(unsigned i = 2; i <= n; ++i) {
      cerr << "k = " << i << ", n = " << n << " (s = " << s << ")" << endl;
      cerr << "----------------------------------------------------" << endl;
      
      for(unsigned j = 0; j < i; ++j)
         cerr << j << "\t";
      cerr << endl;
      
      for(unsigned j = 0; j < i; ++j)
         cerr << P_INDEX(j, false, n, i) << "\t";
      cerr << endl;
      for(unsigned j = 0; j < i; ++j)
         cerr << P_INDEX(j, true,  n, i) << "\t";
      cerr << endl;
   }
   exit(0);*/
   
   memset(pdfs, 0, sizeof(real_t) * (k + 1));
   
   // pdfs[s] represents the probability density with which this path of length
   // k was actually generated; because we're combining pdfs with multiple 
   // importance sampling, we really only care about the relative probability 
   // densities (since each weight in multiple importance sampling is a ratio 
   // of densities -- their ratio is all that matters).  to make the 
   // computation (of all possible probability densities with which this 
   // k-length path could have been sampled) easier, we define p(s) to be one 
   // and compute all other probability densities relative to p(s) by using 
   // the quick / easy-to-compute ratios p(s+1)/p(s) and p(s+1)/p(s)
   pdfs[s] = 1;
   
   // p(i) is known; compute ratio p(i+1)/p(i) to determine p(s+1)...p(k)
   for(unsigned i = s; i < k; ++i) {
      real_t num = 1, den = 1;
      
      // p(i+1)/p(i) = p^l(i+1)/p^e(i+1)
      //             = p(xi+1 | xi, xi-1) / p(xi+1 | xi+2, xi+3)
      
      if (i == 0) {
         const PathVertex &y = m_vertices[P_INDEX(i + 0, false, n, k)];
         
         num = y.pL;
      } else {
         const PathVertex &y = m_vertices[P_INDEX(i - 1, false, n, k)];
         
         num = y.pdfL * y.GL;
      }
      
      if (i == k - 1) {
         const PathVertex &z = m_vertices[P_INDEX(i + 0, true,  n, k)];
         
         den = z.pE;
      } else {
         const PathVertex &z = m_vertices[P_INDEX(i + 1, true,  n, k)];
         
         den = z.pdfE * z.GE;
      }
      
      if (num == 0 || den == 0)
         break;
      
      ASSERT(i + 1 <= k);
      pdfs[i + 1] = pdfs[i] * (num / den);
   }
   
   // p(i) is known; compute ratio p(i-1)/p(i) to determine p(s-1)...p(0)
   for(unsigned i = s; i >= 1; --i) {
      real_t num = 1, den = 1;
      
      if (i == 1) {
         const PathVertex &y = m_vertices[P_INDEX(i - 1, false, n, k)];
         
         den = y.pL;
      } else {
         const PathVertex &y = m_vertices[P_INDEX(i - 1, false, n, k)];
         
         den = y.pdfL * y.GL;
      }
      
      if (i == k) {
         const PathVertex &z = m_vertices[P_INDEX(i - 1, true,  n, k)];
         
         num = z.pE;
      } else {
         const PathVertex &z = m_vertices[P_INDEX(i + 0, true,  n, k)];
         
         num = z.pdfE * z.GE;
      }
      
      if (num == 0 || den == 0)
         break;
      
      ASSERT(i - 1 <= k);
      pdfs[i - 1] = pdfs[i] * (num / den);
   }
   
   // handle specular vertices
   for(unsigned i = k; i--;) {
      const unsigned indexE = P_INDEX(i, true,  n, k);
      const unsigned indexL = P_INDEX(i, false, n, k);
      
      ASSERT(pdfs[i] >= 0);
      
      // any path with a connecting edge containing a specular vertex 
      // automatically has its contribution set to zero because these paths 
      // are impossible to deal with numerically (dirac BSDFs and pdfs).
      // instead of counting their contribution, we will instead account for 
      // them using one of the other k-1 sampling techniques (in which 
      // this specular vertex is not part of the connecting edge)
      if (m_vertices[indexE].bsdf->isSpecular() || 
          m_vertices[indexL].bsdf->isSpecular())
      {
         pdfs[i] = pdfs[i + 1] = 0;
      }
   }
}

#if 0
bool Path::_samplePathVertex(bool adjoint) {
   PathVertex &v = m_vertices[(adjoint ? 0 : length() - 1)];
   SurfacePoint *pt;
   real_t t;
   
   // sample direction from endpoint's BSDF
   const Event &e = v.bsdf->sample();
   const Vector3 &wo = e;
   
   { // trace ray to determine new samplepoint
      const Ray ray(v.pt->position, wo);
      
      pt = new SurfacePoint();
      t  = m_renderer->getScene()->getIntersection(ray, *pt);
      
      if (!pt->init(ray, t)) {
         safeDelete(pt);
         return false;
      }
   }
   
   { // initialize new point  
            const real_t cosWo = v.pt->normal.dot(wo);
      const real_t pPSA  = v.bsdf->getPd(e) / ABS(cosWo);
      const real_t G     = ABS(cosWo * pt->normal.dot(-wo)) / (t * t);
      ASSERT(pPSA > 0);
      ASSERT(G    > 0);
      
      const SpectralSampleSet &fs    = v.bsdf->evaluate(wo);
      const SpectralSampleSet &alpha = fs / pPSA;
      
      if (adjoint) {
         m_vertices.push_front(PathVertex(pt, alpha * v.alphaE, G, -wo));
      } else {
         m_vertices.push_back(PathVertex(pt, alpha * v.alphaL));
         v.G = G;
      }
      
      v.fs   = fs;
      v.pPSA = pPSA;
   }
   
   return true;
}
#endif


#if 0
   ASSERT(actualS <= k && n <= k);
   
   memset(pdfs, 0, sizeof(real_t) * (n + 1));
   
   unsigned s = actualS;             // index of current light vertex
   unsigned t = k - n + actualS - 1; // index of current eye vertex
   real_t pL  = 1;
   real_t pE  = 1;
   
   if (actualS == 0) {
      ASSERT(t == k - n);
      pdfs[0] = !m_vertices[0].bsdf->isSpecular();
      
      pE = m_vertices[t].pE;
   } else if (actualS == n) {
      pdfs[s - 1] = !m_vertices[s - 1].bsdf->isSpecular();
      
      pL = m_vertices[s - 1].pL;
   } else {
      pdfs[s - 1] = !(m_vertices[s - 1].bsdf->isSpecular() || 
                      m_vertices[t].bsdf->isSpecular());
      
      pL = m_vertices[s - 1].pL;
      pE = m_vertices[t].pE;
   }
   
   for(unsigned i = actualS; i <= n && pdfs[i - 1] > 0; ++i, s = t++) {
      ASSERT(s < k && t <= k);
      
      const PathVertex &y = m_vertices[s];
      const real_t E      = (t < k ? m_vertices[t].pE : 1);
      
      if (pL == 0 || y.pE == 0 || y.bsdf->isSpecular())
         break;
      
      pdfs[i] = (pdfs[i - 1] * y.pL * E) / (pL * y.pE);
      pL      = y.pL;
   }
   
   s = actualS - 2;
   t = actualS - 1;
   
   for(; actualS > 0 && t > 0 && pdfs[t] > 0; t = s--) {
      ASSERT(t < k && s < k);
      
      const PathVertex &y = m_vertices[s];
      const PathVertex &z = m_vertices[t];
      
      if (z.pL == 0 || pE == 0 || y.bsdf->isSpecular() || z.bsdf->isSpecular())
         break;
      
      pdfs[s] = (pdfs[t] * y.pL * z.pE) / (z.pL * pE);
      pE      = y.pE;
   }
}

real_t Path::getPd(unsigned s, unsigned t) const {
   // note s and t are exclusive; u and v are inclusive
   const unsigned k = length();
   const unsigned u = s - 1; // index of last light subpath vertex
   const unsigned v = k - t; // index of last eye   subpath vertex
   ASSERT(s + t <= k);
   
   real_t pL = 1, pE = 1;
   if (s > 0)
      pL = m_vertices[u].pL;
   else if (m_vertices[v].pt->shape->getSurfaceArea() == 0)
      pL = 0;
   
   if (t > 0)
      pE = m_vertices[v].pE;
   else if (m_vertices[u].pt->shape->getSurfaceArea() == 0)
      pE = 0;
   
   const real_t pdf = pL * pE;
   ASSERT(pdf >= 0);
   
   return pdf;
}
#endif

#if 0
void Path::getPds(unsigned k, unsigned s, real_t *pdfs) const {
   const unsigned n = length();
   ASSERT(s <= n && k <= n);
   s = MIN(s, k);
   
   memset(pdfs, 0, sizeof(real_t) * (k + 1));
   
   // pdfs[s] represents the probability density with which this path of length
   // k was actually generated; because we're combining pdfs with multiple 
   // importance sampling, we really only care about the relative probability 
   // densities (since each weight in multiple importance sampling is a ratio 
   // of densities -- their ratio is all that matters).  to make the 
   // computation (of all possible probability densities with which this 
   // k-length path could have been sampled) easier, we define p(s) to be one 
   // and compute all other probability densities relative to p(s) by using 
   // the quick / easy-to-compute ratios p(s+1)/p(s) and p(s+1)/p(s)
   pdfs[s] = 1;
   
   // p(i) is known; compute ratio p(i+1)/p(i) to determine p(s+1)...p(k)
   for(unsigned i = s; i < k; ++i) {
      real_t num = 1, den = 1;
      
      if (i == 0) {
         const PathVertex &v = m_vertices[P_INDEX(i, n, s, k)];
         
         num = v.pL;
         den = v.pdfE * v.GE;
      } else if (i < k - 1) {
         const PathVertex &u = m_vertices[P_INDEX(i - 1, n, s, k)];
         const PathVertex &w = m_vertices[P_INDEX(i + 1, n, s, k)];
         
         num = u.pdfL * u.GL;
         den = w.pdfE * w.GE;
      } else { // i == k - 1
         ASSERT(i == k - 1);
         
         const PathVertex &u = m_vertices[P_INDEX(i - 1, n, s, k)];
         const PathVertex &v = m_vertices[P_INDEX(i, n, s, k)];
         
         num = u.pdfL * u.GL;
         den = v.pE;
      }
      
      if (num == 0 || den == 0)
         break;
      
      ASSERT(i + 1 <= k);
      pdfs[i + 1] = pdfs[i] * (num / den);
   }
   
   // p(i) is known; compute ratio p(i-1)/p(i) to determine p(s-1)...p(0)
   for(unsigned i = s; i >= 1; --i) {
      real_t num = 1, den = 1;
      
      if (i == 1) {
         const PathVertex &u = m_vertices[P_INDEX(i - 1, n, s, k)];
         const PathVertex &v = m_vertices[P_INDEX(i, n, s, k)];
         
         num = v.pdfE * v.GE;
         den = u.pL;
      } else if (i < k) {
         const PathVertex &t = m_vertices[P_INDEX(i - 2, n, s, k)];
         const PathVertex &v = m_vertices[P_INDEX(i, n, s, k)];
         
         num = v.pdfE * v.GE;
         den = t.pdfL * t.GL;
      } else { // i == k
         ASSERT(i == k);
         
         const PathVertex &t = m_vertices[P_INDEX(i - 2, n, s, k)];
         const PathVertex &u = m_vertices[P_INDEX(i - 1, n, s, k)];
         
         num = u.pE;
         den = t.pdfL * t.GL;
      }
      
      if (num == 0 || den == 0)
         break;
      
      ASSERT(i - 1 <= k);
      pdfs[i - 1] = pdfs[i] * (num / den);
   }
   
   unsigned oldIndex = n;
   
   // handle specular vertices
   for(unsigned i = k; i--;) {
      const unsigned index = P_INDEX(i, n, s, k);
      
      ASSERT(index < oldIndex);
      oldIndex = index;
      
      // any path with a connecting edge containing a specular vertex 
      // automatically has its contribution set to zero because these paths 
      // are impossible to deal with numerically (dirac BSDFs and pdfs).
      // instead of counting their contribution, we will instead account for 
      // them using one of the other k-1 sampling techniques (in which 
      // this specular vertex is not part of the connecting edge)
      if (m_vertices[index].bsdf->isSpecular())
         pdfs[i] = pdfs[i + 1] = 0;
   }
}
#endif
 
/*#ifdef DEBUG
   //cerr << (*this) << endl << endl;
   
   real_t pE = m_vertices[k-1].pE;
   real_t pL = m_vertices[0].pL;
   
   // path integrity check
   for(unsigned i = k; i--;) {
      PathVertex &x = m_vertices[i];
      
      // sanity check to ensure all values are within acceptable expected ranges
      ASSERT(x.GL >= 0);
      ASSERT(x.GE >= 0);
      
      ASSERT(x.alphaL >= SpectralSampleSet::black());
      ASSERT(x.alphaE >= SpectralSampleSet::black());
      
      ASSERT(x.fs >= SpectralSampleSet::black());
      
      ASSERT(x.pdfL >= 0);
      ASSERT(x.pdfE >= 0);
      
      ASSERT(x.pL >= 0);
      ASSERT(x.pE >= 0);
      
      // more thorough checks to ensure path is logically sound
      if (i > 0) {
         PathVertex &y = m_vertices[i - 1];
         
         ASSERT(y.GL     == x.GE);
         //ASSERT(y.alphaL >= x.alphaL - SpectralSampleSet::fill(0.001));
         //ASSERT(y.pL     >= x.pL - 0.001);
         
         // only the first light vertex is allowed to be an emitter; if a light
         // subpath strikes an emitter after the first vertex, it should act as
         // a normal surface w/ respect to reflectance, not as an emitter
         ASSERT(x.bsdf != x.pt->emitter);
         
         PathVertex &v = m_vertices[k - i - 1];
         pL *= v.pdfL * v.GL;
         
         ASSERT(EQ(pL, m_vertices[k - i].pL));
      } else {
         ASSERT(x.bsdf == x.pt->emitter && x.pt->emitter->isEmitter());
      }
      
      if (i < k - 1) {
         PathVertex &z = m_vertices[i + 1];
         
         ASSERT(z.GE     == x.GL);
         //ASSERT(z.alphaE >= x.alphaE - SpectralSampleSet::fill(0.001));
         //ASSERT(z.pE     >= x.pE - 0.001);
         
         // only the first eye vertex is allowed to be a sensor; if an eye 
         // subpath strikes a sensor after the first vertex, it should act as 
         // a normal surface w/ respect to reflectance, not as a sensor
         ASSERT(x.bsdf != x.pt->sensor);
         
         pE *= z.pdfE * z.GE;
         
         ASSERT(EQ(pE, x.pE));
      } else {
         ASSERT(x.bsdf == x.pt->sensor && x.pt->sensor->isSensor());
      }
   }
   
   if (s > 0 && t > 0 && (0 == invalid || 3 == invalid)) {
      for(unsigned i = k + 1; i--;) {
         const SpectralSampleSet &c = getContribution(k - i, i);
         
         if (invalid) {
            ASSERT(c == SpectralSampleSet::black());
         } else {
            //ASSERT(c.getSum() > 0);
         }
      }
   }
#endif*/
   
#if 0
   for(unsigned i = 0; i < k; ++i) {
      PathVertex &v = m_vertices[i];
      
      if (i > 0)
         v.tE = (m_vertices[i - 1].pt->position - v.pt->position).getMagnitude();
      else
         v.tE = 0;
      
      if (i < k - 1)
         v.tL = (m_vertices[i + 1].pt->position - v.pt->position).getMagnitude();
      else
         v.tL = 0;
   }
#endif

}

