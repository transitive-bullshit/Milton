/**<!-------------------------------------------------------------------->
   @file   MLTMarkovProcess.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Models a random walk throughout the space of all light paths in a scene 
   as a Markov process (where state transitions are "memoryless" in that they 
   only depend on the previous state), with state transitions governed by 
   the Metropolis-Hastings algorithm (states are specific paths of light). 
   
   @see MLTPathMutation
   <!-------------------------------------------------------------------->**/

#include "MLTMarkovProcess.h"
#include <RenderOutput.h>
#include <ColorUtils.h>
#include <Camera.h>
#include <QtCore/QtCore>
#include <mlt.h>

namespace milton {

MLTMarkovProcess::~MLTMarkovProcess() {
   safeDelete(m_mutation);
}

void MLTMarkovProcess::init() {
   ASSERT(m_weight >=  0);
   safeDelete(m_mutation);
   
   m_mutation = new MLTAggregatePathMutation(m_renderer);
   m_mutation->init();
   
   m_maxDepth = m_renderer->getMaxDepth();
   m_maxConsequtiveRejections = m_renderer->getMaxConsequtiveRejections();
}

extern unsigned s_ind;
void MLTMarkovProcess::run() {
   PointSample sample, tentative;
   Path pathY(m_renderer);
   unsigned noVisits = 0;
   unsigned maxVisits = 40;
   
   _initSample(m_path, sample);
   
   do {
      // sample a new path
      real_t alpha = m_mutation->mutate(m_path, pathY);
      real_t theta = 1;
      alpha        = MIN(create_real(1), theta * alpha);
      
      /*if (pathY.length() == 5 && pathY[1].bsdf->isSpecular()&&!pathY[2].bsdf->isSpecular()&&pathY[3].bsdf->isSpecular())
      {
         pathY.clear();
         alpha = 0;
#warning ""
#warning ""
#warning ""
#warning "WARNING: biased -- put LSDSE paths back in!!!"
#warning ""
#warning ""
#warning ""
      }*/
      
      /*if (m_path.length() == 5 && m_path[1].bsdf->isSpecular()&&!m_path[2].bsdf->isSpecular()&&m_path[3].bsdf->isSpecular()) {
         cerr << "-----------------------------------------------------" << endl;
         cerr << "TENTATIVE alpha: " << alpha << ", mutation: " << s_ind << endl;
      }*/
#if 0
      if (alpha>0&&pathY.length() >= 5 && pathY[1].bsdf->isSpecular()) {
         unsigned i = 2;
         
         while(i < pathY.length() && pathY[i].bsdf->isSpecular())
            ++i;
         
         if (i == pathY.length() - 2) {
            cerr << "-----------------------------------------------------" << endl;
            cerr << "TENTATIVE alpha: " << alpha << ", mutation: " << s_ind 
                 << endl << pathY << endl << endl;
            cerr << m_path << endl << endl;
            //abort();
         }
      }
#endif
      
      // ensure path generated is valid
      if (alpha == 0 || pathY.length() < 2 || 
          (m_maxDepth > 0 && pathY.length() > m_maxDepth))
      {
         pathY.clear();
         alpha = 0;
      }
      
      _initSample(pathY, tentative);
      
      ++noVisits;
      if (m_maxConsequtiveRejections <= 0 || 
          noVisits < m_maxConsequtiveRejections)
      {
         sample.update = m_primary;
         
         _addSample(sample,    1 - alpha, false);
         _addSample(tentative, alpha,     true);
      }
      
      // transition with probability alpha
      if (Random::sample(0, 1) < alpha) {
         if (m_primary && noVisits > maxVisits) {
            maxVisits = noVisits;
            cerr << "noVisits: " << noVisits << ", mutation: " << s_ind 
               << ", alpha: " << alpha << endl;
            cerr << m_path << endl << endl << endl;
         }
         
         noVisits = 0;
         
         m_path = pathY;
         sample = tentative;
         ASSERT(!sample.value.getValue<SpectralSampleSet>().isZero());
      }
   } while(1);
}

void MLTMarkovProcess::_initSample(Path &path, PointSample &sample) {
   SpectralSampleSet radiance = path.getRadiance();
   const Camera *camera = m_renderer->getCamera();
   
   if (path.length() < 2 || radiance.isZero()) {
      sample.value = Event(SpectralSampleSet::black());
      return;
   }
   
#if 0
   RgbaHDR i = radiance.getRGB();
   RGBtoXYZ(i.r, i.g, i.b, i.r, i.g, i.b);
   //XYZtoxyY(i.r, i.g, i.b, i.r, i.b, i.g);
   
   i.r /= i.g;
   i.b /= i.g;
   i.g = 1.0;
   
   XYZtoRGB(i.r, i.g, i.b, i.r, i.g, i.b);
   radiance[0].value = i[0];
   radiance[1].value = i[1];
   radiance[2].value = i[2];
   
   radiance *= m_weight;
#else
   radiance *= m_weight / radiance.getRGB().luminance();
#endif
   
   // TODO: remove horrible hack
   //for(unsigned i = radiance.getN(); i--;)
   //   radiance[i].value = CLAMP(radiance[i].value, 0, 1);
   
   const Point3 &p = path[path.length() - 2].pt->position;
   sample.position = camera->getProjection(p);
   sample.value    = Event(radiance);
}

void MLTMarkovProcess::_addSample(const PointSample &sample, real_t prob, 
                                  bool tentative)
{
   if (prob > 0) {
      const SpectralSampleSet &val = sample.value.getValue<SpectralSampleSet>();
      PointSample temp(sample.position, Event(val * prob, sample.value), 
                       sample.update, false);
      
      if (tentative)
         m_renderer->getOutput()->addPropposed(sample);
      m_renderer->getOutput()->addSample(temp);
   }
}

}

