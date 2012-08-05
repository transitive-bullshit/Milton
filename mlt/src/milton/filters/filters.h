/**<!-------------------------------------------------------------------->
   @file   filters.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Convenience header which includes all Filter implementations
   <!-------------------------------------------------------------------->**/

#ifndef FILTERS_H_
#define FILTERS_H_

// Filter implementations (approximately in order from worst to best)
// Note -- it's a bit funny that better filters tend to have longer names..
#include <filters/BoxFilter.h>
#include <filters/TriangleFilter.h>
#include <filters/GaussianFilter.h>
#include <filters/MitchellFilter.h>
#include <filters/LanczosSincFilter.h>

#include <filters/ProgressiveFilterValue.h>

#endif // FILTERS_H_

