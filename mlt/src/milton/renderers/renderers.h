/**<!-------------------------------------------------------------------->
   @file   renderers.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Convenience header which includes all public renderer headers
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_RENDERERS_H_
#define MILTON_RENDERERS_H_

// concrete Renderer implementations
#include <renderers/renderers/BidirectionalPathTracer.h>
#include <renderers/renderers/OpenGLRenderer.h>
#include <renderers/renderers/PathTracer.h>
//#include <renderers/renderers/PhotonMapper.h>
#include <renderers/renderers/RayCaster.h>
#include <renderers/renderers/WhittedRayTracer.h>

// renderer utils
#include <renderers/utils/Path.h>
#include <renderers/utils/PathVertex.h>
#include <renderers/utils/IPathGenerator.h>
//#include <renderers/utils/Photon.h>
//#include <renderers/utils/PhotonMap.h>

#include <renderers/mlt/mlt.h>
#include <renderers/outputs/outputs.h>
#include <renderers/generators/generators.h>
#include <renderers/tonemaps/tonemaps.h>

// miscellaneous
#include <renderers/PointSample.h>
#include <renderers/DirectIllumination.h>

#endif // MILTON_RENDERERS_H_

