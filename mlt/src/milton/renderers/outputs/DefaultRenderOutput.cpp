/**<!-------------------------------------------------------------------->
   @file   DefaultRenderOutput.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Records point samples from a renderer and reconstructs the underlying 
   image, using a default reconstruction reconstruction filter
   <!-------------------------------------------------------------------->**/

#include "DefaultRenderOutput.h"
#include "PointSample.h"

#include <filters.h>
#include <QtCore/QtCore>

namespace milton {

DefaultRenderOutput::DefaultRenderOutput(Image *output)
   : ReconstructionRenderOutput(output, new LanczosSincFilter(2))
{ }

}

