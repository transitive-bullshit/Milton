/**<!-------------------------------------------------------------------->
   @file   mlt.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Convenience header which includes all public headers related to 
   Milton's implementation of Metropolis Light Transport (MLT)
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_MLT_H_
#define MILTON_MLT_H_

#include <renderers/mlt/MLTRenderer.h>
#include <renderers/mlt/MLTMarkovProcess.h>

// path mutations
#include <renderers/mlt/MLTAggregatePathMutation.h>
#include <renderers/mlt/MLTBidirPathMutation.h>
#include <renderers/mlt/MLTLensSubpathMutation.h>
#include <renderers/mlt/MLTPerturbationPathMutation.h>

#endif // MILTON_MLT_H_

