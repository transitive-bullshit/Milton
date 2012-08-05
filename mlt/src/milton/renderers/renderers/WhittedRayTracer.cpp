/**<!-------------------------------------------------------------------->
   @file   WhittedRayTracer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Classic Whitted-style raytracer; for a concise introduction to 
   raytracing, see http://www.raytracing.co.uk/study/ray_intro.htm
   <!-------------------------------------------------------------------->**/

#include "WhittedRayTracer.h"
#include <DirectIllumination.h>
#include <SurfacePoint.h>
#include <Material.h>
#include <Camera.h>
#include <Scene.h>
#include <QtCore/QtCore>
#include <Ray.h>

namespace milton {

void WhittedRayTracer::init() {
   m_noIndirectSamples = getValue<unsigned>("noIndirectSamples", 
                                            m_noIndirectSamples);
   
   m_maxDepth = getValue<unsigned>("maxDepth", m_maxDepth);
   m_ambient  = getValue<SpectralSampleSet>("ambient", m_ambient);
   
   RayTracer::init();
}

void WhittedRayTracer::_evaluate(const Ray &ray, SpectralSampleSet &outRadiance, 
                                 PropertyMap &data)
{
   const unsigned depth = data.getValue<unsigned>("depth", 0u);
   
   // base case for recursion depth
   if (depth >= m_maxDepth)
      return;
   
   // find closest intersection
   SurfacePoint pt;
   const real_t t = m_scene->getIntersection(ray, pt);
   
   // lazily initialize SurfacePoint and return if no intersection
   // also return if we hit an emitter after the first bounce, 
   // ensuring we don't real_t-count direct illumination
   if (!pt.init(ray, t)) {
      outRadiance += m_scene->getBackgroundRadiance(ray.direction);
      return;
   }
   
   // evaluate emitted radiance from intersection point, making sure not to 
   // double-count direct illumination
   if (pt.emitter->isEmitter()) {
      if (depth == 0) {
         outRadiance += pt.emitter->getLe(-ray.direction);
      } else {
         const real_t emittedContribution = 
            data.getValue<real_t>("emittedContribution", 0);
         
         if (emittedContribution > EPSILON) {
            outRadiance += pt.emitter->getLe(-ray.direction) * 
               ((pt.shape->getSurfaceArea() * 
                 ABS(pt.normal.dot(-ray.direction) * emittedContribution)) / 
                (t * t));
         }
      }
   }
   
   // estimate direct illumination
   outRadiance += m_directIllumination->evaluate(pt);
   
   // ambient term
   outRadiance += m_ambient;
   
   if (pt.bsdf->isSpecular()) {
      // estimate indirect illumination
      for(unsigned i = m_noIndirectSamples; i--;) {
         // sample the BSDF for an exitent direction
         const Event &event = pt.bsdf->sample();
         const Vector3 &wo  = event;
         if (wo == Vector3::zero())
            continue; // absorbed
         
         const SpectralSampleSet &fs = pt.bsdf->evaluate(wo);
         const real_t pdf = pt.bsdf->getPd(event);
         
         // if the surface has a non-zero bsdf for the given reflected vector
         if (!fs.isZero()) {
            SpectralSampleSet Li;
            
            // estimate indirect illumination by recurring
            data["depth"] = depth + 1;
            data["emittedContribution"] = (pt.bsdf->isSpecular() ? 
                                           pt.normal.dot(wo) : 0);
            
            _evaluate(Ray(pt.position, wo), Li, data);
            
            // attenuate incident illumination Li by reflectivity of surface 
            // in exitant direction (bsdf) and add its contribution to the 
            // exitant radiance
            outRadiance += (fs * Li / (pdf * m_noIndirectSamples));
         }
      }
   }
}

}

