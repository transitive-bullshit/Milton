/**<!-------------------------------------------------------------------->
   @file   DirectIllumination.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Interface for estimating direct illumination from all luminaires in 
   a scene to a given surface point on a surface
   <!-------------------------------------------------------------------->**/

#include "DirectIllumination.h"
#include <generators.h>

#include <SurfacePoint.h>
#include <PointSample.h>
#include <QtCore/QtCore>
#include <Material.h>
#include <Renderer.h>
#include <ShapeSet.h>
#include <Scene.h>
#include <BSDF.h>
#include <Ray.h>

namespace milton {

DirectIllumination::~DirectIllumination() {
   safeDelete(m_generator);
}

void DirectIllumination::init() {
   ASSERT(m_renderer);
   
   m_noDirectSamples = getValue<unsigned>("noDirectSamples", 1);
   
   const std::string &directSampleGenerator = 
      getValue<std::string>("directSampleGenerator", std::string("jittered"));
   
   m_generator = SampleGenerator::create(directSampleGenerator);
   m_generator->init();
}

SpectralSampleSet DirectIllumination::evaluate(SurfacePoint &pt) {
   return evaluate(pt, m_noDirectSamples);
}

SpectralSampleSet DirectIllumination::evaluate(SurfacePoint &pt, 
                                      unsigned reqNoDirectSamples)
{
   ASSERT(reqNoDirectSamples > 0);
   
   Scene *scene = m_renderer->getScene();
   ShapeSet &lights = *scene->getLights();
   SpectralSampleSet Li;
   
   // with probability 1, a specular surface's BSDF will be zero for all random
   // rays shot towards light sources, so don't bother
   if (pt.bsdf->isSpecular())
      return SpectralSampleSet::black();
   /*{
      const Event &event = pt.bsdf->sample();
      const Vector3 &wo  = event;
      
      if (wo == Vector3::zero())
         return SpectralSampleSet::black();
      
      Ray ray(pt.position, wo);
      SurfacePoint pt2;
      
      const real_t t = scene->getIntersection(ray, pt2);
      
      if (!pt2.init(ray, t))
         return SpectralSampleSet::black();
      
      Shape *light = pt2.shape;
      
      if (!pt2.emitter->isEmitter())
         return SpectralSampleSet::black();
      
      const real_t lightSurfaceArea = light->getSurfaceArea();
      
      const real_t cosWo = ABS(pt.normal.dot(wo));
      const real_t cosWi = ABS(pt2.normal.dot(-wo));
      
      const real_t pdf   = pt.bsdf->getPd(event);
      const SpectralSampleSet &fs = pt.bsdf->evaluate(wo) / pdf;
      
      return lightSurfaceArea * pt2.emitter->getLe(-wo) * (cosWo * cosWi) / (t * t);
   }*/
   
   for(unsigned i = lights.size(); i--;) {
      Shape *light = lights[i];
      SpectralSampleSet direct;
      
      // unless we were to have rally oddly-shaped lights, this should be fine
      if (pt.shape == light)
         continue;
      
      const real_t lightSurfaceArea = light->getSurfaceArea();
      const bool isPoint = (lightSurfaceArea <= EPSILON);
      unsigned noDirectSamples = (isPoint ? 1 : reqNoDirectSamples);
      
      PointSampleList samples;
      m_generator->generate(samples, Viewport(noDirectSamples));
      ASSERT(samples.size() == noDirectSamples);
      
      // average incident radiance from current light source over N samples
      FOREACH(PointSampleListIter, samples, sample) {
         SurfacePoint lightPt;
         light->getPoint(lightPt, UV(sample->position[0], 
                                     sample->position[1]));
         ASSERT(lightPt.emitter);
         
         Vector3 wo     = (lightPt.position - pt.position);
         const real_t t = wo.normalize();
         
         const real_t cosWo = ABS(pt.normal.dot(wo));
         const real_t cosWi = ABS(lightPt.normal.dot(-wo));
         const SpectralSampleSet &fr = pt.bsdf->evaluate(wo);
         
         // if point is not in shadow with respect to the current surface point
         // on the current light source, then add its contribution
         if (cosWo > 0 && cosWi > 0 && fr != SpectralSampleSet::black()
             && !scene->intersects(Ray(pt.position, wo), t - EPSILON))
         {
            const SpectralSampleSet &frLi = fr * lightPt.emitter->getLe(-wo);
            
            direct += (frLi * cosWo * cosWi) / (t * t);
         }
      }
      
      if (!isPoint)
         direct *= (lightSurfaceArea / noDirectSamples);
      
      Li += direct;
   }
   
   return Li;
}

}

