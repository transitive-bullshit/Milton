/**<!-------------------------------------------------------------------->
   @file   ReconstructionRenderOutput.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Records point samples from a renderer and attempts to reconstruct the 
   underlying image by filtering samples with a reconstruction filter
   <!-------------------------------------------------------------------->**/

#include "ReconstructionRenderOutput.h"
#include "PointSample.h"

#include <SpectralSampleSet.h>
#include <Renderer.h>
#include <filters.h>
#include <QtCore/QtCore>

namespace milton {

ReconstructionRenderOutput::ReconstructionRenderOutput(Image *output, 
                                                       KernelFilter *f) 
   : RenderOutput(output), m_filter(f)
{ }

ReconstructionRenderOutput::~ReconstructionRenderOutput() {
   safeDelete(m_filter);
}

void ReconstructionRenderOutput::addSample(PointSample &sample) {
   if (m_isMLT && sample.update) {
      const unsigned seconds = m_parent->getTimer().elapsed();
      
      if (seconds >= m_seconds + 5) {
         cout << m_parent->getElapsedTime() << " elapsed" << endl;
         m_seconds = seconds;
         
         sample.update = true;
         sample.save   = true;
      } else {
         sample.update = false;
         sample.save   = false;
      }
   }
   
   if (NULL == m_filter) {
      RenderOutput::addSample(sample);
      return;
   }
   
   ASSERT(m_output);
   const unsigned width  = m_output->getWidth();
   const unsigned height = m_output->getHeight();
   const unsigned half   = (m_filter->getWidth() >> 1);
   unsigned col, row;
   
   m_viewport.getBin(sample.position, col, row);
   
   ASSERT(row < width);
   ASSERT(col < height);
   
   const real_t centerX = sample.position[0] * width;
   const real_t centerY = sample.position[1] * height;
   
   const unsigned maxY  = MIN(height - 1, row + half);
   const unsigned maxX  = MIN(width  - 1, col + half);
   const bool update    = sample.update;
   
   // progressive, normalized filter
   for(unsigned y = (row > half ? row - half : 0); y <= maxY; ++y) {
      for(unsigned x = (col > half ? col - half : 0); x <= maxX; ++x) {
         const real_t weight = m_filter->evaluate(
            Vector2(x - centerX, y - centerY)
         );
		 
         if (weight > 0) {
            sample.update = (update && x == col && y == row);
            
            _addSample(y, x, sample, weight, true);
         }
      }
   }
}

void ReconstructionRenderOutput::init() {
   RenderOutput::init();
   
   ASSERT(m_output);
   ASSERT(m_progressiveValues);
   
   if (NULL == m_filter) {
      const std::string &filter = 
         getValue<std::string>("filter", "null");
      
      if (filter != "null")
         m_filter = KernelFilter::create(filter, *this);
   }
   
   if (m_filter)
      m_filter->init();
}

}

