/**<!-------------------------------------------------------------------->
   @file   HilbertSampleGenerator.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   December 2006
   
   @brief
      Recursive space-filling L-System represented graphically
   
   @see http://en.wikipedia.org/wiki/Hilbert_curve
   <!-------------------------------------------------------------------->**/

#include "HilbertSampleGenerator.h"
#include <QtCore/QtCore>

namespace milton {

template <class SG>
void HilbertSG<SG>::generate(PointSampleList &outSamples, 
                             const Viewport &viewport)
{
#if 0
   const unsigned width   = viewport.getWidth();
   const unsigned height  = viewport.getHeight();
   
   const real_t binWidth  = viewport.getInvWidth();
   const real_t binHeight = viewport.getInvHeight();
   
   for(unsigned depth = 2; depth < 8; ++depth) {
      const std::string &system = process(depth);
      
      cerr << depth << endl;
      cerr << system << endl;
      unsigned x = 0, y = height - 1;
      unsigned lenX = width, lenY = height;
      real_t angle = M_PI / 2.0; // 90 degree turns
      real_t theta = 0;
      
      for(unsigned i = depth; i--;) {
         lenX >>= 1;
         lenY >>= 1;
      }
      
      for(unsigned i = 0; i < system.length(); ++i) {
         const char cur = system[i];
         
         switch(cur) {
            case 'F':
               {
                  unsigned newX = (unsigned)round(x + lenX * cos(theta));
                  unsigned newY = (unsigned)round(y + lenY * sin(theta));
                  
                  if (newX == x) {
                     for(unsigned j = MIN(newY, y); j < MAX(newY, y); ++j) {
                        SG::_addSample(PointSample(min0 + x, min1 + j), 
                                       outSamples);
                     }
                  } else if (newY == y) {
                     for(unsigned j = MIN(newX, x); j < MAX(newX, x); ++j) {
                        SG::_addSample(PointSample(min0 + j, min1 + y), 
                                       outSamples);
                     }
                  } else {
                     cerr << x << ", " << y << " vs " << newX << ", " << newY << endl;
                     ASSERT(0);
                  }
                  
                  x = newX;
                  y = newY;
                  
                  cerr << "(" << x << ", " << y << ")" << endl;
               }
               break;
            case '-':
               theta += angle;
               break;
            case '+':
               theta -= angle;
               break;
            default:
               break;
         }
      }
   }
#endif
}

template <class SG>
std::string HilbertSG<SG>::_processRule(char c) {
   if (c == 'L')
      return "+RF-LFL-FR+";
   if (c == 'R')
      return "-LF+FRF+FL-";
   
   return "";
}

}

// force explicit template specialization of DissolveSampleGenerator and 
// DissolveSampleGeneratorThread
#include <generators.h>

namespace milton {
   DECLARE_SAMPLE_GENERATOR(Hilbert);
}

