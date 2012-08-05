/**<!-------------------------------------------------------------------->
   @class  MiltonParams
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      MiltonApp parameters parsed from the commandline and overrideable by 
   MiltonApp users / subclasses
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_PARAMS_H_
#define MILTON_PARAMS_H_

#include <common/common.h>

namespace milton {

DECLARE_STL_TYPEDEF(std::vector<std::string>, StringList);

class RenderOutput;

struct MiltonParams {
   bool enableGui;     // enable / disable Qt gui
                       // note: X11 will not be started if gui is disabled
   bool startRender;   // whether or not to initiate renders right away
   bool forcePreview;  // whether or not to force OpenGL previews
                       // (disables rendering)
   bool verbose;       // extra debugging printouts (disabled by default)
   
   std::string output; // if gui is disabled, default output
   unsigned    seed;   // random number generator seed
   
   StringList scenefiles;
   
   inline MiltonParams()
      : enableGui(true), startRender(true), forcePreview(false), 
        verbose(false), output("out.png"), seed(0)
   { }
};

}

#endif // MILTON_PARAMS_H_

