/**
  * @author Travis Fischer (fisch0920@gmail.com)
  * @date   Fall 2008
  * 
  * @brief
  *   Visualizes different sample generation patterns on a 2D domain by 
  * generating a collection of images at different scales and sample-
  * generation methods.
  */
#include <SampleGenerators.h>
#include <Random.h>
#include <Image.h>
#include <QtGui>

#include <sstream>
using namespace std;

int main(int argc, char** argv) {
   Random::init();
   
   const unsigned N = 256;
   
   char *sampleGenerators[] = {
      "uniform", "stochastic", "jittered", NULL
   };
   
   for(unsigned size = 1; size <= 32; size *= 2) {
      Domain domain(1, 1);
      
      domain.insert("noSamples", size * size);
      
      char *str = NULL;
      unsigned index  = 0;
      
      while(NULL != (str = sampleGenerators[index++])) {
         SampleGenerator *generator = SampleGenerator::create(domain, str);
         PointSampleList samples;
         
         generator->generate(samples, size * size);
         
         Image *image = new Image(N, N);
         Pixel *data = image->getData();

         //memset(data, 0xff, sizeof(Pixel) * N * N);
         
         for(unsigned i = N * N; i--;)
            data[i].data = 0x00ffffff;
         
         for(unsigned i = size + 1; i--;) {
            unsigned row = MIN(N - 1, floor((N * i) / (double)(size)));
            ASSERT(row < N);
            
            for(unsigned x = N; x--;) {
               data[row * N + x].data = 0x00000000;
               data[x * N + row].data = 0x00000000;
            }
         }
         
         unsigned counter = 0;
         FOREACH(PointSampleListIter, samples, iter) {
            int row = floor(N * iter->position[1]);
            int col = floor(N * iter->position[0]);
            
            for(unsigned x = MAX(row - 2, 0); x <= MIN(row + 2, N - 1); ++x) {
               for(unsigned y = MAX(col - 2, 0); y <= MIN(col + 2, N - 1); ++y) {
                  ASSERT(x < N && y < N);
                  data[y * N + x].data = 0x00ff0000;
               }
            }
            
            ++counter;
         }
         ASSERT(counter == size * size);
         
         ostringstream s;
         s << "out/sampleGenerator_" << size << "_" << str << ".png";
         assert(image->save(s.str().c_str()));
         safeDelete(image);
      }
   }
   
   return 0;
}

