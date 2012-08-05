/**<!-------------------------------------------------------------------->
   @file   RayTracer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Abstract ray tracing engine, with the following concrete 
   implementations:  WhittedRayTracer, StochasticRayTracer
   <!-------------------------------------------------------------------->**/

#include "RayTracer.h"
#include <PointSample.h>
#include <Camera.h>
#include <QtCore/QtCore>
#include <Ray.h>

namespace milton {

void RayTracer::sample(PointSample &outSample) {
   ASSERT(m_scene);
   ASSERT(m_camera);
   PropertyMap data;
   
   const Ray &ray = m_camera->getWorldRay(outSample.position);
   SpectralSampleSet radiance;
   
   _evaluate(ray, radiance, data);
   outSample.value.setValue(radiance);
   
   /*if ((unsigned)(outSample.position[0]) == 278 && 
       (unsigned)(outSample.position[1]) == 71)
   { // misc debugging
      cerr << radiance << endl;
   }*/
}

}

