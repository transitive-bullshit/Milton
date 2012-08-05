/**<!-------------------------------------------------------------------->
   @file   SuperSampleGenerator.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
     Brute force super sampling, where sub-bin (sub-pixel) sampling is done 
   via an auxillary SampleGenerator
   <!-------------------------------------------------------------------->**/

#include "SuperSampleGenerator.h"
#include <PointSampleRenderer.h>
#include <ResourceManager.h>
#include <Random.h>
#include <QtCore/QtCore>
using namespace std;

#define NO_SUB_GENERATORS      (133)

namespace milton {

template <class SG>
void SuperSG<SG>::init() {
   SG::init();
   
   m_noSuperSamples = 
      boost::any_cast<unsigned>(SG::getValue("noSuperSamples", 4u));
   Viewport subviewport(m_noSuperSamples > 0 ? m_noSuperSamples : 4);
   
   if (m_noSuperSamples > 0)
      cerr << "samples per pixel: " << m_noSuperSamples << endl;
   else 
      cerr << "samples per pixel: " << "infinite" << endl;
   
   SampleGenerator *sg = 
      SampleGenerator::create(m_subGeneratorType);
   
   sg->init();
   for(unsigned i = NO_SUB_GENERATORS; i--;) {
      m_samples.push_back(PointSampleList());
      sg->generate(m_samples.back(), subviewport);
   }
   
   safeDelete(sg);
}

template <class SG>
void SuperSG<SG>::generate(PointSampleList &outSamples, 
                           const Viewport &viewport)
{
   ASSERT(m_samples.size() == NO_SUB_GENERATORS);
   
   const unsigned width   = viewport.getWidth();
   const unsigned height  = viewport.getHeight();
   
   const real_t binWidth  = viewport.getInvWidth();
   const real_t binHeight = viewport.getInvHeight();
   unsigned s = 0;
   
   do {
      unsigned index = (NO_SUB_GENERATORS - 1);
      
      for(unsigned i = 0; i < height; ++i) {
         for(unsigned j = 0; j < width; ++j) {
            const PointSample &p = m_samples[index][s % m_samples[index].size()];
            const real_t x = j * binWidth  + p.position[0] * binWidth;
            const real_t y = i * binHeight + p.position[1] * binHeight;
            
            const bool update = (j == width - 1);
            SG::_addSample(
               PointSample(x, y, update, update && (i == height - 1)), 
               outSamples
            );
            
            if (index-- == 0)
               index = NO_SUB_GENERATORS - 1;
         }
      }
      
      ++s;
      
      if (m_noSuperSamples > 0) {
         cerr << "progress: " << (unsigned)(100 * ((real_t)(s) / m_noSuperSamples)) 
              << "% complete; ";
      } else {
         cerr << s << " sample" << (s == 1 ? " " : "s") << " per pixel; ";
      }
      
      cerr << SG::getRenderer()->getElapsedTime() << " elapsed" << endl;
   } while(m_noSuperSamples == 0 || s < m_noSuperSamples);
}

}

// force explicit template specialization of SuperSampleGeneratorThread
#include <generators.h>

namespace milton {
   // Only declare threaded version of super sample generator
   template class SuperSG<SampleGeneratorThread>;
}

