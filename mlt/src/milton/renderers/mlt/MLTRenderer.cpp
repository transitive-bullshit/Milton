/**<!-------------------------------------------------------------------->
   @file   MLTRenderer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Metropolis Light Transport (MLT) renderer based on Veach & Guibas, 1997.
   MLT is currently the most efficient light transport algorithm known for
   robustly rendering arbitrarily complex scenes that many other rendering 
   algorithms would consider difficult for various reasons (eg. strong 
   indirect illumination, small cracks / holes, etc). At its core, MLT is an 
   application of the general-purpose Metropolis-Hastings sampler, applied in 
   the context of trying to sample all possible paths of light starting at an 
   emitter and ending at a virtual camera sensor in a carefully tuned way such 
   that the probability density of simulating any one path is proportional to 
   that path's relative contribution to what that virtual sensor would end up 
   "seeing" in the virtual scene.
   
   @see MLTMarkovProcess
   @see MLTPathMutation
   <!-------------------------------------------------------------------->**/

#include "MLTRenderer.h"
#include <BidirectionalPathTracer.h>
#include <ResourceManager.h>
#include <RenderOutput.h>
#include <PointSample.h>
#include <Camera.h>
#include <QtCore/QtCore>
#include <Ray.h>
#include <mlt.h>

namespace milton {

MLTRenderer::~MLTRenderer() {
   if (m_pathGenerator) {
      m_pathGenerator->setCamera(NULL);
      m_pathGenerator->setScene (NULL);
      
      safeDelete(m_pathGenerator);
   }
}

void MLTRenderer::init() {
   if (!m_initted) {
      PointSampleRenderer::init();
      
      if (m_pathGenerator) {
         m_pathGenerator->setCamera(NULL);
         m_pathGenerator->setScene (NULL);
         
         safeDelete(m_pathGenerator);
      }
      
      m_pathGenerator = 
         new BidirectionalPathTracer(m_output, m_camera, m_scene);
      
      ASSERT(m_output);
      m_output->setParent(this);
      
      // customizable parameters
// TODO:  "what is good default value for maxDepth? 10 20 40?"
      m_maxDepth = getValue<unsigned>("mltMaxDepth", 10);
      m_maxConsequtiveRejections = getValue<unsigned>("mltMaxConsequtiveRejections", 500);
      
      cout << "random seed: " << Random::s_seed << endl;
   }
}

void MLTRenderer::render() {
   if (!m_initted)
      init();
   
   QMutexLocker lock(&m_renderMutex);
   m_output->init();
   
   unsigned noRenderThreads = 0;
   unsigned noInitialPaths  = 0;
   
   { // parse parameters
      noRenderThreads  = getValue<unsigned>("noRenderThreads", 1u);
      noRenderThreads += (noRenderThreads == 0);
      
      noInitialPaths   = getValue<unsigned>("mltNoInitialPaths",  10000u);
      noInitialPaths  += (noInitialPaths  == 0);
   }
   
   // initialize seed paths and estimate 'b,' the total radiant flux falling 
   // on the film plane
   PathList seedPaths;
   RealList W;
   real_t weight = _initSeedPaths(seedPaths, W, noInitialPaths);
   
   // initialize timer to begin counting
   m_timer.reset();
   
   cout << "rendering with " << noRenderThreads 
      << (noRenderThreads == 1 ? " thread" : " threads") 
      << endl << endl;
   
   std::vector<MLTMarkovProcess*> processes;
   for(unsigned i = noRenderThreads; i--;) {
      const unsigned pathIndex = Random::sampleNormalizedCDF(&W[0], W.size());
      ASSERT(W[pathIndex] > 0);
      
      MLTMarkovProcess *process = 
         new MLTMarkovProcess(this, seedPaths[pathIndex], weight, (i == 0));
      
      processes.push_back(process);
      
      process->init();
      process->start();
   }
   
   // TODO: have different ways of stopping or run individual 
   // processes forever?
   
   for(unsigned i = noRenderThreads; i--;) {
      MLTMarkovProcess *process = processes[i];
      
      while(!process->wait());
      safeDelete(process);
   }
}

real_t MLTRenderer::_initSeedPaths(PathList &outPaths, RealList &outW, 
                                   const unsigned noInitialPaths)
{
   ASSERT(noInitialPaths > 0);
   
   cout 
      << endl 
      << "generating initial seed paths (noInitialPaths = " 
      << noInitialPaths << ")" 
      << endl;
   
   Path path(this);
   unsigned n = 0;
   real_t sum = 0;
   
   do {
      for(unsigned i = noInitialPaths; i--;) {
         path.clear();
         
#if 0
         m_pathGenerator->generateE(path);
         const unsigned length = path.length();
         
         if (length >= 2 && path.front().isEmitter()) {
            Path aggregate(this);
            aggregate.append(path);
            ASSERT(aggregate.length() == path.length());
            
            const SpectralSampleSet &c = aggregate.getContribution(0, aggregate.length());
            const real_t f = c.getRGB().luminance();
            
            outPaths.push_back(aggregate);
            outW.push_back(f);
         } else {
            path.clear();
            outPaths.push_back(path);
            outW.push_back(0);
         }
#else
         (void) m_pathGenerator->generate(path);
         const unsigned length = path.length();
         
         if (m_maxDepth > 0 && path.length() > m_maxDepth) {
            ++i;
            continue;
         }
         
         for(unsigned k = 2; k <= length; ++k) {
            for(unsigned t = k + 1, s = 0; t--; ++s) {
               Path p2 = path.left(s);
               bool v  = p2.append(path.right(t));
               
               ASSERT(s + t == k);
               ASSERT(s >= 0 && s <= k);
               ASSERT(t >= 0 && t <= k);
               ASSERT(p2.length() == k);
               
               if (!v) {
                  outW.push_back(0);
                  outPaths.push_back(Path(this));
               } else {
                  const real_t f = path.getContribution(s, t).getRGB().luminance();
                  
                  outW.push_back(f);
                  outPaths.push_back(p2);
               }
            }
         }
#endif
      }
      
      n = outW.size();
      sum = 0;
      
      for(unsigned i = n; i--;)
         sum += outW[i];
   } while(n == 0 || sum <= 0);
   ASSERT(n > 0);
   
   // normalize weights
   for(unsigned i = n; i--;)
      outW[i] /= sum;
   
#ifdef DEBUG
   for(unsigned i = n; i--;) {
      if (outW[i] > 0) {
         ASSERT(outPaths[i].length() >= 2);
         
         const SpectralSampleSet &radiance = outPaths[i].getRadiance();
         if (radiance.isZero())
            cerr << outPaths[i] << endl;
         ASSERT(!radiance.isZero());
      }
   }
#endif
   
#if 0
   for(unsigned i = outPaths.size(); i--;) {
      if (outW[i] > 0) {
         Path p = outPaths[i];
         
         if (p.length() == 4) {
            outPaths.clear();
            outPaths.push_back(p);
            
            outW.clear();
            outW.push_back(1);
            break;
         }
      }
   }
#endif
   
   cerr << "sum: " << sum << ", n: " << n << ", weight: " << (sum / n) << endl;
   return (sum / n);
}

}

