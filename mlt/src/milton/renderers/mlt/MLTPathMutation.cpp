/**<!-------------------------------------------------------------------->
   @file   MLTPathMutation.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Abstract superclass for all path mutations used by MLTRenderer, whose 
   main task is to sample mutated versions of a path and return the 
   probability density of accepting the mutation.
   
   @see MLTAggregatePathMutation
   @see MLTBidirPathMutation
   @see MLTLensSubpathMutation
   @see MLTPerturbationPathMutation
   <!-------------------------------------------------------------------->**/

#include "MLTAggregatePathMutation.h"
#include <QtCore/QtCore>
#include <mlt.h>

namespace milton {

void MLTPathMutation::init()
{ }

}

