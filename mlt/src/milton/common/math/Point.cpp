/**<!-------------------------------------------------------------------->
   @file   Point.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides basic functionality for a homogeneous point
   <!-------------------------------------------------------------------->**/

// Point.h can be found in /course/cs123/include/algebra
#include <Point.h>
#include <math.h>

namespace milton {

/// Homogenizes the point passed in such that the Nth value ("w") is one
void homogenize(Point3 &p) {
   const real_t w = p[3];
   
   if (w == 0) {
      cerr << "\nError: " << "Attempting to homogenize a point when w = 0" << endl << endl; 
   } else if (w != 1) {
      for(unsigned i = 3; i--;)
         p[i] /= w;
      
      // Set w to 1 explicitly instead of dividing by itself to avoid errors 
      // in floating point precision
      p[3] = 1;
   }
}

// force explicit template specialization
template struct Point<3, real_t>;

}

