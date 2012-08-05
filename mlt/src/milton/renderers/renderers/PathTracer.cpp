/**<!-------------------------------------------------------------------->
   @file   PathTracer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Unbiased path tracer with support for efficient direct illumination
   <!-------------------------------------------------------------------->**/

#include "PathTracer.h"
#include <DirectIllumination.h>
#include <SurfacePoint.h>
#include <Material.h>
#include <Camera.h>
#include <Random.h>
#include <Scene.h>
#include <QtCore/QtCore>
#include <Ray.h>

namespace milton {

void PathTracer::init() {
   m_efficientDirect = getValue<bool>("efficientDirect", true);
   
   RayTracer::init();
}

void PathTracer::_evaluate(const Ray &ray, SpectralSampleSet &outRadiance, 
                           PropertyMap &data)
{
   const unsigned depth = data.getValue<unsigned>("depth", 0);
   const unsigned index = data.getValue<unsigned>("iorIndex", (unsigned) Random::sampleInt(0, 3));
   
   // find closest intersection
   SurfacePoint pt;
   const real_t t = m_scene->getIntersection(ray, pt);
   
   // lazily initialize SurfacePoint and return if no intersection
   if (!pt.init(ray, t)) {
      outRadiance += m_scene->getBackgroundRadiance(ray.direction);
      return;
   }
   
   pt.iorIndex = index + 1;
   if (pt.bsdf->isSpecular())
      data["specular"] = true;
   
   // evaluate emitted radiance from intersection point, making sure 
   // not to double-count direct illumination
   if (pt.emitter->isEmitter()) {
      if (!m_efficientDirect || (depth == 0)) {
         outRadiance += pt.emitter->getLe(-ray.direction);
      } else {
         const real_t emittedContribution = 
            data.getValue<real_t>("emittedContribution", 0);
         
         // note: this is a bit messy and really belongs in DirectIllumination, 
         // but it is an optimization to place it here s.t. the reflected 
         // ray from a specular surface only needs to be traced through the 
         // scene once (we're in this case only if a specular bounce landed on 
         // a light source)
         if (emittedContribution > EPSILON) {
            outRadiance += pt.emitter->getLe(-ray.direction) * 
               (pt.shape->getSurfaceArea() / 
                (t * t));
                 //ABS(pt.normal.dot(-ray.direction)/* * emittedContribution*/)
         }
      }
   }
   
   // estimate direct illumination
   if (m_efficientDirect)
      outRadiance += m_directIllumination->evaluate(pt);
   
   // sample the BSDF for an exitant direction
   const Event &event = pt.bsdf->sample();
   const Vector3 &wo  = event;
   
   if (wo == Vector3::zero())
      return; // absorbed
   
   const real_t pdf = pt.bsdf->getPd(event);
   const SpectralSampleSet &fs = pt.bsdf->evaluate(wo) / pdf;
   
   // record russian roulette probability for terminating random walk
   // note: russian roulette increases variance noticeably, so don't 
   // use it until several bounces have passed
   const real_t pCont = 
      (depth < 2 ? !fs.isZero() : MIN(.95, fs[fs.getMaxSample()].value));
   
   // russian roulette to terminate walk depending on albedo of surface
   if (Random::sample(0, 1) < pCont) {
      SpectralSampleSet Li;
      
      // estimate indirect illumination by recurring
      data["depth"] = depth + 1;
      data["emittedContribution"] = (pt.bsdf->isSpecular() ? 
                                     pt.normal.dot(wo) : 0);
      
      _evaluate(Ray(pt.position, wo), Li, data);
      
      // attenuate indirect illumination estimate Li by reflectivity of surface 
      // in exitant direction according to the bsdf and add its contribution to 
      // the exitant radiance
      outRadiance += fs * Li / pCont;
      
      if (depth == 0 && data.getValue<bool>("specular", false)) {
         for(unsigned i = outRadiance.getN(); i--;) {
            if (i != index)
               outRadiance[i].value = 0;
            else 
               outRadiance[i].value *= outRadiance.getN();
         }
      }
   }
}

}

