/**<!-------------------------------------------------------------------->
   @file   PhotonMapper.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008

   @brief
      Implementation of Henrik Wann Jensen's photon mapping algorithm. Photon 
   Mapping is a two-pass global illumination algorithm in which photons are 
   first traced from light sources and stored in a global kd-tree called the 
   photon map. Final gathering is then performed by tracing paths from the eye 
   and using photons from the photon map to approximate irradiance at points 
   in the scene.
    
   @see Photon
   @see PhotonMap
   @see Jensen's book, "Realistic Image Synthesis Using Photon Mapping" (2001)
      for further details
   <!-------------------------------------------------------------------->**/

#include "PhotonMapper.h"
#include "PhotonTracer.h"
#include "PhotonMap.h"
#include <renderers/utils/Path.h>

#include <renderers/DirectIllumination.h>
#include <utils/ResourceManager.h>
#include <core/SurfacePoint.h>
#include <materials/Material.h>
#include <cameras/Camera.h>
#include <core/Scene.h>
#include <core/Ray.h>
#include <QtCore>

PhotonMapper::~PhotonMapper() {
   safeDelete(m_photonTracer);
   safeDelete(m_diffusePhotonMap);
   safeDelete(m_causticPhotonMap);
}

void PhotonMapper::init() {
   if (NULL == m_photonTracer) {
      RayTracer::init();
      
      { // initialize photon maps
         const unsigned diffuseNoGatherPhotons = 
            getValue<unsigned>("diffuseNoGatherPhotons", 500);
         const unsigned causticNoGatherPhotons = 
            getValue<unsigned>("causticNoGatherPhotons", 300);
         
         const real_t diffuseGatherRadius = 
            getValue<real_t>("diffuseGatherRadius", 50.0);
         const real_t causticGatherRadius = 
            getValue<real_t>("causticGatherRadius", 50.0);
         
         m_diffusePhotonMap = 
            new PhotonMap(diffuseNoGatherPhotons, diffuseGatherRadius);
         m_causticPhotonMap = 
            new PhotonMap(causticNoGatherPhotons, causticGatherRadius);
         
         m_photonTracer     = new PhotonTracer();
      }
      
      { // trace diffuse and caustic photons
         _tracePhotons();
      }
      
      { // build kd-Trees
         cerr << "building kd-Tree for " << 
            m_diffusePhotonMap->size() << " diffuse photons" << endl;
         m_diffusePhotonMap->init();
         
         cerr << "building kd-Tree for " << 
            m_causticPhotonMap->size() << " caustic photons" << endl;
         m_causticPhotonMap->init();
      }
   }
}

void PhotonMapper::_tracePhotons() {
   const unsigned diffuseNoPhotons = 
      getValue<unsigned>("diffuseNoPhotons", 60000);
   const unsigned causticNoPhotons = 
      getValue<unsigned>("causticNoPhotons", 60000);
   
   unsigned diffuseNoEmitted = 0;
   unsigned causticNoEmitted = 0;
   
   cerr << "tracing " << diffuseNoPhotons << " diffuse photons" << endl;
   while(diffuseNoEmitted < diffuseNoPhotons && m_diffusePhotonMap->size() < diffuseNoPhotons) {
      _tracePhoton(false);
      ++diffuseNoEmitted;
   }
   
   cerr << "tracing " << causticNoPhotons << " caustic photons" << endl;
   while(causticNoEmitted < causticNoPhotons && m_causticPhotonMap->size() < causticNoPhotons) {
      _tracePhoton(true);
      
      ++causticNoEmitted;
   }
   
   // normalize photon powers
   m_diffusePhotonMap->scalePhotonPower(1.0 / diffuseNoEmitted);
   m_causticPhotonMap->scalePhotonPower(1.0 / causticNoEmitted);
   
   //m_diffusePhotonMap->save("diffuse.png", m_camera);
   //m_causticPhotonMap->save("caustic.png", m_camera);
}

void PhotonMapper::_tracePhoton(bool caustic) {
   //const std::string &pathNotation = (caustic ? "LS+D" : "LD+");
   Path path(this);
   
   m_photonTracer->generateL(path, caustic);//pathNotation);
   
   if (path.length() <= 2)
      return;
   
   PhotonMap *photonKdTree = 
      (caustic ? m_causticPhotonMap : m_diffusePhotonMap);
   
   for(unsigned i = 2; i < path.length(); ++i) {
      const PathVertex &v = path[i];
      
      if (!v.bsdf->isSpecular()) {
         Photon p(v.pt->position, -v.wi, v.alphaL);
         
         photonKdTree->addPhoton(p);
      }
   }
}

void PhotonMapper::_evaluate(const Ray &ray, SpectralSampleSet &outRadiance, 
                             PropertyMap &data)
{
   const unsigned depth = data.getValue<unsigned>("depth", 0);
   
   // find closest intersection
   SurfacePoint pt;
   const real_t t = m_scene->getIntersection(ray, pt);
   
   // lazily initialize SurfacePoint and return if no intersection
   if (!pt.init(ray, t))
      return;
   
   // evaluate emitted radiance from intersection point
   // note (tfischer): depth check to not double-count direct illumination 
   // is not needed (and should *not* be included); we're only here after 
   // the first bounce if we hit a specular surface -- which wouldn't have 
   // taken into account direct illumination cause it would've been 
   // guaranteed to be zero as far as random sampling goes
   outRadiance += pt.emitter->getLe(-ray.direction);
   
   // Split the rendering equation into four parts:
   // direct illumination, specular reflection, caustics, 
   // soft indirect illumination
   
   // estimate specular (reflection / refraction) contribution
   // L(S|D)*SE
   if (pt.bsdf->isSpecular()) {
      const Event &event = pt.bsdf->sample();
      const Vector3 &wo  = event;
      
      if (wo != Vector3::zero()) {
         const real_t pdf = pt.bsdf->getPdf(event);
         const SpectralSampleSet &fs = pt.bsdf->getBSDF(wo) * ABS(pt.normal.dot(wo)) / pdf;
         SpectralSampleSet Li;
         
         // estimate indirect specular illumination by recurring
         data.getValue<unsigned>("depth") = depth + 1;
         _evaluate(Ray(pt.position, wo), Li, data);
         
         // attenuate incident illumination Li by reflectivity of surface 
         // in exitant direction (bsdf) and add its contribution to the 
         // exitant radiance
         outRadiance += fs * Li;
      }
   } else {
      // calculate direct illumination 
      // L?D
      outRadiance += m_directIllumination->evaluate(pt);
      
      // compute direct caustics
      // LS+DE
      outRadiance += m_causticPhotonMap->getIrradiance(pt);
      
      // compute indirect illumination
      // LD+DE
      outRadiance += m_diffusePhotonMap->getIrradiance(pt);
   }
}

