/**<!-------------------------------------------------------------------->
   @file   RenderOutput.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Records point samples from a renderer which may be used to construct 
   an output image on a local or distributed machine
   <!-------------------------------------------------------------------->**/

#include "RenderOutput.h"
#include "PointSample.h"
#include "Renderer.h"

#include <SpectralSampleSet.h>
#include <ToneMap.h>
#include <miltonimage.h>
#include <QtCore/QtCore>

namespace milton {

RenderOutput::~RenderOutput() {
   safeDelete(m_output);
   safeDelete(m_tonemap);
   
   safeDeleteArray(m_progressiveValues);
   safeDeleteArray(m_proposed);
   safeDeleteArray(m_locks);
   safeDeleteArray(m_noSamples);
}

void RenderOutput::init() {
   if (NULL == m_output) {
      const unsigned width  = getValue<unsigned>("width",  480u);
      const unsigned height = getValue<unsigned>("height", 480u);
      
      m_output = new RgbaImage(width, height);
   }
   
   const unsigned width  = m_output->getWidth();
   const unsigned height = m_output->getHeight();
   const unsigned size   = width * height;
   m_viewport.setSize(width, height);
   
   safeDelete(m_tonemap);
   safeDeleteArray(m_progressiveValues);
   safeDeleteArray(m_proposed);
   safeDeleteArray(m_locks);
   safeDeleteArray(m_noSamples);
   
   m_progressiveValues = new ProgressiveFilterValue<SpectralSampleSet>[size];
   const std::string &tonemap = getValue<std::string>(
      "tonemap", std::string("default")
   );
   
   m_tonemap   = ToneMap::create(tonemap, *this);
   m_proposed  = new unsigned long[size];
   
   m_locks     = new QMutex[width];
   m_noSamples = new unsigned long[width];
   
   m_isMLT          = (m_parent ? m_parent->isMLT() : false);
   m_filterProposed = (m_parent ? m_parent->getValue<bool>("mltFilterProposed", true) : true);
   
   m_seconds = 0;
   memset(m_proposed, 0, sizeof(unsigned long) * size);
}

void RenderOutput::finalize()
{ } // purposefully empty

void RenderOutput::addSample(PointSample &sample) {
   unsigned row, col;
   
   m_viewport.getBin(sample.position, col, row);
   _addSample(row, col, sample, 1, true);
}

void RenderOutput::_addSample(const unsigned row, const unsigned col, 
                              PointSample &sample, const real_t weight, 
                              bool lock)
{
   ASSERT(m_progressiveValues);
   ASSERT(m_noSamples);
   ASSERT(m_output);
   const unsigned width = m_viewport.getWidth();
   
   if (lock)
      _lockPixel(row, col);
   
   ProgressiveFilterValue<SpectralSampleSet> &p = 
      m_progressiveValues[row * width + col];
   
   p.addSample(sample.value.getValue<SpectralSampleSet>(), weight);
   m_output->setPixel(row, col, p.getValue());
   
   ++m_noSamples[col % width];
   
   if (lock)
      _unlockPixel(row, col);
}

void RenderOutput::addPropposed(const PointSample &sample) {
   unsigned row, col;
   const unsigned width = m_viewport.getWidth();
   m_viewport.getBin(sample.position, col, row);
   
   _lockPixel(row, col);
   ++m_proposed[row * width + col];
   _unlockPixel(row, col);
}

void RenderOutput::setImage(Image *image) {
   ASSERT(image);
   
   if (image == m_output)
      return;
   
   safeDeleteArray(m_progressiveValues);
   safeDelete(m_output);
   
   m_output = image;
   m_viewport.setSize(m_output->getWidth(), m_output->getHeight());
   
   const unsigned size = m_output->getSize();
   m_progressiveValues = new ProgressiveFilterValue<SpectralSampleSet>[size];
}

RgbaImage *RenderOutput::getFinalizedOutput() {
   ASSERT(m_progressiveValues);
   ASSERT(m_output);
   
   const unsigned width  = m_output->getWidth();
   const unsigned height = m_output->getHeight();
   
   HDRImage *hdrOutput = new HDRImage(width, height);
   RgbaHDR *dest = hdrOutput->getData();
   
   // accumulate samples
   unsigned noSamples = 0;
   for(unsigned i = width; i--;) {
      m_locks[i].lock();
      noSamples += m_noSamples[i];
   }
// TODO:  "possible deadlock acquiring locks in inconsistent order here and in ReconstructionRenderOutput"
   
   // differences in pixel intensities when rendering with MLT occur with 
   // respect to the relative number of samples which occur in a given pixel.
   // when rendering with MLT, we therefore need to divide each estimated 
   // pixel's value by the total number of samples taken so far, in effect, 
   // creating a histogram of intensities based on their relative contribution
   // to the radiant flux incident on all of the film plane
   if (m_isMLT) {
      // 2 samples (tentative and real) for each actual Xi sample in the random walk
      const real_t invNoSamples = (2.0) / noSamples; // divided by 25
      
      if (m_filterProposed) {
         const int half    = 1;
         const int support = 2 * half + 1;
         
         unsigned long *temp = new unsigned long[support * support];
         
         for(int i = height; i--;) {
            for(int j = width; j--;) {
               //real_t sum = 0, mid = 0;
               unsigned n = 0;
               
               // median filter
               for(int y = -half; y <= half; ++y) {
                  int row = CLAMP(i + y, 0, (int)height - 1);
                  
                  for(int x = -half; x <= half; ++x) {
                     int col = CLAMP(j + x, 0, (int)width - 1);
                     
                     temp[n++] = m_proposed[row * width + col];
                     //sum += 1;
                     //mid += 1 * m_proposed[row * width + col];
                  }
               }
               
               //avg /= sum;
               
               real_t mid = 0;
               if (n > 0) {
                  std::sort(temp, temp + n);
                  
                  mid = temp[n / 2];
                  
                  if ((n & 1) == 0)
                     mid = temp[n / 2 - 1];//0.5 * (mid + temp[n / 2 - 1]);
               }
               
               const unsigned offset = i * width + j;
               const unsigned long proposed = m_proposed[offset];
               const real_t scale = 
                  (mid > 0 && proposed > 0 ? mid / proposed : 1);
               
               const SpectralSampleSet &val = m_progressiveValues[offset].numerator;
               dest[offset] = (val * (scale * invNoSamples)).getRGB();
            }
         }
         
         safeDeleteArray(temp);
      } else {
         for(unsigned i = hdrOutput->getSize(); i--;) {
            const SpectralSampleSet &val = m_progressiveValues[i].numerator;
            
            /*if ((i / m_output->getWidth()) == m_output->getHeight() / 2) {
               const real_t v = ((val * invNoSamples)[0].value + (val * invNoSamples)[1].value + (val * invNoSamples)[2].value) / 3.0;
               
               cout << v << ", ";
            }*/
            
            dest[i] = (val * invNoSamples).getRGB();
         }
      }
   } else {
      for(unsigned i = hdrOutput->getSize(); i--;) {
         const SpectralSampleSet &val = m_progressiveValues[i].getValue();
         dest[i] = val.getRGB();
      }
   }
   
   for(unsigned i = width; i--;)
      m_locks[i].unlock();
   
   // perform tonemapping
   RgbaImage *output = m_tonemap->map(hdrOutput);
   safeDelete(hdrOutput);
   
   return output;
}

void RenderOutput::_lockPixel  (unsigned row, unsigned col) {
   m_locks[col % m_viewport.getWidth()].lock();
}

void RenderOutput::_unlockPixel(unsigned row, unsigned col) {
   m_locks[col % m_viewport.getWidth()].unlock();
}

}

