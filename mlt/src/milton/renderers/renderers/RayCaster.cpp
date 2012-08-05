/**<!-------------------------------------------------------------------->
   @file   RayCaster.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Simple, non-recursive raycaster
   <!-------------------------------------------------------------------->**/

#include "RayCaster.h"
#include <DirectIllumination.h>
#include <SurfacePoint.h>
#include <Material.h>
#include <Camera.h>
#include <Scene.h>
#include <QtCore/QtCore>
#include <Ray.h>

namespace milton {

void RayCaster::init() {
   m_ambient = getValue<SpectralSampleSet>("ambient", m_ambient);
   
   RayTracer::init();
}

void RayCaster::_evaluate(const Ray &ray, SpectralSampleSet &outRadiance, 
                          PropertyMap &data)
{
   // find closest intersection
   SurfacePoint pt;
   const real_t t = m_scene->getIntersection(ray, pt);
   
   // lazily initialize SurfacePoint and return if no intersection
   if (!pt.init(ray, t)) {
      outRadiance += m_scene->getBackgroundRadiance(ray.direction);
      return;
   }
   
   // evaluate emitted radiance from intersection point
   outRadiance += pt.emitter->getLe(-ray.direction);
   
   // estimate direct illumination
   outRadiance += m_directIllumination->evaluate(pt);
   
   // ambient term
   outRadiance += m_ambient;
}

}

