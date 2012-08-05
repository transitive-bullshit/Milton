/**<!-------------------------------------------------------------------->
   @file   BidirectionalBidirectionalPathTracer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Unbiased bidirectional path tracer with support for efficient direct 
   illumination
   <!-------------------------------------------------------------------->**/

#include "BidirectionalPathTracer.h"
#include <DirectIllumination.h>
#include <RenderOutput.h>
#include <SurfacePoint.h>
#include <Material.h>
#include <Viewport.h>
#include <HDRImage.h>
#include <Rgba.h>
#include <Camera.h>
#include <Scene.h>
#include <QtCore/QtCore>
#include <Path.h>
#include <Ray.h>
#include <sstream>

namespace milton {

// comment / uncomment to disable / enable the radiance hack..
#define RADIANCE_HACK

//#define DEBUG_PATHS

#if 1

void BidirectionalPathTracer::sample(PointSample &sample) {
   Path path(this);
   Path eye(this);
   
   /*if (m_images == NULL) {
      m_images = new HDRImage*[10];
      m_filters = new ProgressiveFilterValue<SpectralSampleSet>*[10];
      for(unsigned i = 10; i--;) {
         m_images[i] = new HDRImage(480, 480);
         m_filters[i] = new ProgressiveFilterValue<SpectralSampleSet>[480 * 480];
      }
   }*/
   
   { // initialize first vertex to current samplepoint on camera's lens
      SurfacePoint *pt = new SurfacePoint();
      const real_t pA = 1.0; // 'sampling' uniformly on film plane
      
      m_camera->getPoint(*pt, UV(sample.position));
      eye.prepend(PathVertex(pt, 1, pA));
   }
   
   bool debug = 
#ifdef DEBUG_PATHS
      true;
#else
   // 240, 71  = direct k = 2 from light source
   // 240, 200 = back wall
   // 241, 361 = lower back wall
      ((unsigned)(sample.position[0] * m_output->getViewport().getWidth()) == 240 && 
       (unsigned)(sample.position[1] * m_output->getViewport().getHeight()) == 360);
#endif
   
   if (debug) {
      cerr << "eye" << endl;
      cerr << "----------------------------------------------------------------" << endl;
   }
   
   generateE(eye);
   
   if (debug)
      cerr << eye << endl;
   
   if (!eye.front().pt->emitter->isEmitter()) {
      if (debug) {
         cerr << "light" << endl;
         cerr << "----------------------------------------------------------------" << endl;
      }
      
      generateL(path);
   }
   
   if (debug)
      cerr << path << endl;
   
   const unsigned actualS = path.length();
   bool valid = path.append(eye);
   
   if (debug) {
      cerr << "complete: " << (valid ? "valid" : "invalid") << endl;
      cerr << "----------------------------------------------------------------" << endl;
      cerr << path << endl << endl;
   }
   
   // TODO: return if invalid or only consider subpaths - how invalid is invalid?
   /*if (!valid) {
      sample.value.setValue(SpectralSampleSet::black());
      return;
   }*/
   
   const unsigned length = path.length() - (!valid);
   const unsigned N = (((length + 1) * (length + 2)) / 2) - 3;
   real_t *pdfs = new real_t[N];
   real_t *sums = new real_t[length];
   unsigned n   = 0;
   SpectralSampleSet L;
   
   // calculate densities of all possible combinations of light and eye 
   // subpaths for use with multiple importance sampling
   for(unsigned k = 2; k <= length; ++k) {
      sums[k - 1] = 0;
      
      real_t *pdf = pdfs + n;
      //path.getPds(k, actualS, pdf);
      
      // s ranges from 0 up to k; t ranges from k down to 0 (all inclusive)
      for(unsigned t = k + 1, s = 0; t--; ++s, ++n) {
         ASSERT(k == s + t);
         
         pdf[s] = path.getPd(s, t, false);
         ASSERT(pdf[s] >= 0);
         
         // power heuristic with beta=2 for multiple importance sampling
         pdf[s] *= pdf[s];
         
         if (debug) {
            cerr << "k = " << k << " (" << s << ", " << t << ") { c = " << path.getContribution(s, t) << ", p = " << pdf[s] << " }" << endl;
         }
         
         sums[k - 1] += pdf[s];
      }
   }
   
   if (debug)
      cerr << endl << endl;
   
   ASSERT(n == N);
   
   if (debug) 
      cerr << endl << path << endl;
   
   // add weighted contributions from all possible combinations of light and 
   // eye subpaths using multiple importance sampling
   for(unsigned k = 2, n = 0; k <= length; ++k) {
      if (sums[k - 1] == 0)
         continue;
      
      const real_t *pdf = pdfs + n;
      
      // s ranges from 0 up to k; t ranges from k down to 0 (all inclusive)
      for(unsigned t = k + 1, s = 0; t--; ++s, ++n) {
         ASSERT(k == s + t);
         
         /*if (debug) {
            const real_t weight = pdf[s] / sums[k - 1];
            const SpectralSampleSet &c = path.getContribution(s, t);
            
            cerr << "k = " << k << " (" << s << ", " << t << ") { C = " << path.getContribution(s, t) << ", p = " << pdf[s] << ", C* = " << weight * c << " }" << endl;
         }*/
         
         // TODO: russian roulette on 'w' if it's really small to avoid extra 
         // intersection test (except for edge cases where intersection test 
         // isn't needed; eg, s=0,t=0,and k=length
         if (pdf[s] > 0) {
            const real_t weight = (pdf[s]) / sums[k - 1];
            const SpectralSampleSet &c = path.getContribution(s, t);
            
            /*if (k < 10) {
               unsigned x, y;
               m_output->getViewport().getBin(sample.position, x, y);
               
               m_filters[k][y * 480 + x].addSample(c, weight);
               m_images[k]->setPixel<RgbaHDR>(y, x, m_filters[k][y * 480 + x].getValue().getRGB());
            }*/
            
            L += weight * c;
         }
      }
   }
   
   safeDeleteArray(pdfs);
   safeDeleteArray(sums);
   
   // TODO: hack
#ifdef RADIANCE_HACK
   for(unsigned i = L.getN(); i--;)
      L[i].value = CLAMP(L[i].value, 0, 1);
#endif
   
   sample.value.setValue(L);
   
   if (debug)
      cerr << "total: " << L << endl;
}

#endif

void BidirectionalPathTracer::finalize() {
   /*for(unsigned i = 10; i--;) {
      stringstream s;
      s << "out" << i << ".png";
      const std::string &f = s.str();
      cerr << "saving " << f << endl;
      m_images[i]->save(f);
   }*/
}

bool BidirectionalPathTracer::generate(Path &light) {
   Path eye(this);
   
   generateE(eye);
   if (!eye.front().pt->emitter->isEmitter())
      generateL(light);
   
   return (light.append(eye));
}

void BidirectionalPathTracer::generateL(Path &light) {
   do {
      const bool roulette = (light.length() > 0);
      
      if (!light.append(roulette))
         break;
   } while(1);
   
   ASSERT(light.length() > 0);
}

void BidirectionalPathTracer::generateE(Path &eye) {
   do {
      const bool roulette = (eye.length() >= 2);
      
      //if (debug)
      //   cerr << eye << endl << endl;
      
      if (!eye.prepend(roulette))
         break;
      
      if (eye.front().pt->emitter->isEmitter())
         break;
   } while(1);
   
   ASSERT(eye.length() > 0);
}

#if 0

void BidirectionalPathTracer::sample(PointSample &sample) {
   Path eye(this);
   
   { // initialize first vertex to current samplepoint on camera's lens
      SurfacePoint *pt = new SurfacePoint();
      const real_t pA = 1.0; // 'sampling' uniformly on film plane
      
      m_camera->getPoint(*pt, UV(sample.position));
      eye.prepend(PathVertex(pt, pA));
   }
   
   real_t pCont = 1;
   SpectralSampleSet L;
   
   do {
      if (!eye.prepend())
         break;
      
      const PathVertex &z = eye[1];
      PathVertex &x       = eye[0];
      
      if (eye.length() == 1)//x.bsdf->isSpecular())
         L += x.pt->emitter->getLe(-x.wi) * z.alphaE;
      //else if (x.pt->emitter->isEmitter())
      //   break;
      
      // estimate direct illumination
      L += m_directIllumination->evaluate(*x.pt.get()) * z.alphaE;
      
      const SpectralSampleSet &fs = z.fs / z.pdfE;
      
      // russian roulette
      pCont = (eye.length() < 12 ? (fs != SpectralSampleSet::black()) : 
               MIN(0.95, fs[fs.getMaxFrequency()]));
      
      if (pCont < Random::sample(0, 1))
         break;
      
      x.alphaE /= pCont;
   } while(1);
   
   sample.value.setValue(L);
   
/*  // bidirectional path contributions
   Path light(this);
   pCont = 1;
   
   bool ret = light.append();
   if (!ret) {
      ASSERT(0 && "sampling initial location on light source failed");
      return;
   }
   
   do {
      if (!light.append())
         break;
      
      const PathVertex &y = light[light.length() - 2];
      PathVertex &x       = light[light.length() - 1];
      
      // TODO: estimate direct contribution to film plane
      const SpectralSampleSet &fs = y.fs / y.pdfL;
      
      // russian roulette
      pCont = (light.length() < 3 ? (fs != SpectralSampleSet::black()) : 
               MIN(1, fs[fs.getMaxFrequency()]));
      
      if (pCont < Random::sample(0, 1))
         break;
      
      x.alphaL /= pCont;
   } while(1);
   
   if (!light.append(eye))
      return;
   
   // note: overall, n = (((length + 1) * (length + 2)) / 2) - 1
   unsigned n = 1;
   
   // add contributions from all possible combinations of light and eye subpaths
   // TODO: multiple importance sampling to weight individual contributions
   for(unsigned k = 1; k <= light.length(); ++k) {
      unsigned s = 0;
      
      // s ranges from 0 up to k; t ranges from k down to 0
      for(unsigned t = k + 1; t--; ++s, ++n) {
         ASSERT(k == s + t);
         
         L += light.getContribution(s, t);
      }
   }
   
   sample.value.setValue(L / n);*/
}

#endif

}

