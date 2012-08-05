/**<!-------------------------------------------------------------------->
   @file   DissolveSampleGenerator.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Cool dissolve effect which generates point samples uniformly over a 
   given 2D domain. The order in which the samples are generated looks random, 
   but it is actually quite deterministic and guaranteed to "visit" every 
   bucket in the domain.
   
   @see A Digital Dissolve Effect by Mike Morton
      (from "Graphics Gems", Academic Press, 1990)
   <!-------------------------------------------------------------------->**/

#include "DissolveSampleGenerator.h"
#include <PointSampleRenderer.h>
#include <QtCore/QtCore>

namespace milton {

// see http://www.scribd.com/doc/7075218/Graphics-Gems-I
static const unsigned randMasks[32] = {
   0x00000001, 0x00000003, 0x00000006, 0x0000000C, 0x00000014, 0x00000030, 
   0x00000060, 0x000000B8, 0x00000110, 0x00000240, 0x00000500, 0x00000CA0, 
   0x00001B00, 0x00003500, 0x00006000, 0x0000B400, 0x00012000, 0x00020400, 
   0x00072000, 0x00090000, 0x00140000, 0x00300000, 0x00400000, 0x00D80000, 
   0x01200000, 0x03880000, 0x07200000, 0x09000000, 0x14000000, 0x32800000, 
   0x48000000, 0xA3000000, 
};

template <class SG>
void DissolveSG<SG>::init() {
   m_noSuperSamples = 
      boost::any_cast<unsigned>(SG::getValue("noSuperSamples", 0u));
   
   if (m_noSuperSamples > 0)
      cerr << "samples per pixel: " << m_noSuperSamples << endl;
   else 
      cerr << "samples per pixel: " << "infinite" << endl;
}

template <class SG>
void DissolveSG<SG>::generate(PointSampleList &outSamples, 
                              const Viewport &viewport)
{
   const real_t binWidth  = viewport.getInvWidth();
   const real_t binHeight = viewport.getInvHeight();
   const unsigned width   = viewport.getWidth();
   const unsigned height  = viewport.getHeight();
   const unsigned size    = width * height;
   
   const unsigned long mask = randMasks[_getBitWidth(size - 1) - 1];
   unsigned long seq = 1;
   unsigned count = 1;
   unsigned s = 0;
   
   do {
      do {
         const real_t y = 0.5 + (seq / width);
         const real_t x = 0.5 + (seq % width);
         
         ++count;
         SG::_addSample(PointSample(x * binWidth, y * binHeight, 
                                    (0 == (count % width))), outSamples);
         
         do {
            seq = (seq >> 1) ^ ((seq & 1) * mask);
         } while(seq >= size);
      } while(seq != 1);  // Loop
      
      SG::_addSample(
         PointSample(0.5 * binWidth, 0.5 * binHeight, true, true), outSamples
      );
      
      ++s;
      cerr << s << " sample" << (s == 1 ? " " : "s") << " per pixel; " 
           << SG::getRenderer()->getElapsedTime() << " elapsed" << endl;
   } while(m_noSuperSamples == 0 || s < m_noSuperSamples);
   
   //cerr << width << ", " << height << endl;
   //cerr << count << " vs " << width * height << endl;
   ASSERT(count == size * m_noSuperSamples);
}

template <class SG>
unsigned DissolveSG<SG>::_getBitWidth(unsigned n) {
   unsigned width = 0;

   while(n) {
      n >>= 1;
      ++width;
   }

   return width;
}

template <class SG>
bool DissolveSG<SG>::generate(Point2 &pt, 
                              const Viewport &viewport, 
                              unsigned seq)
{
   m_seq = seq;
   
   return generate(pt, viewport);
}

template <class SG>
bool DissolveSG<SG>::generate(Point2 &pt, 
                              const Viewport &viewport)
{
   const real_t binWidth  = viewport.getInvWidth();
   const real_t binHeight = viewport.getInvHeight();
   const unsigned width   = viewport.getWidth();
   const unsigned height  = viewport.getHeight();
   const unsigned size    = width * height;
   
   if (m_seq == 1) {
      m_mask = randMasks[_getBitWidth(size - 1) - 1];
      pt = Point2(0.5 * binWidth, 0.5 * binHeight);
   } else {
      const real_t y = 0.5 + (m_seq / width);
      const real_t x = 0.5 + (m_seq % width);
      
      pt = Point2(x * binWidth, y * binHeight);
   }
   
   do {
      m_seq = (m_seq >> 1) ^ ((m_seq & 1) * m_mask);
   } while(m_seq >= size);
   
   return (m_seq != 1);
}

}

// force explicit template specialization of DissolveSampleGeneratorThread
#include <generators.h>

namespace milton {
   // Only declare threaded version of dissolve sample generator
   template class DissolveSG<SampleGeneratorThread>;
}

