/**<!-------------------------------------------------------------------->
   @file   main.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   <!-------------------------------------------------------------------->**/

#include "PathVisualizer.h"
#include <MiltonApp.h>
#include <Qt/QtOpenGL>

using namespace milton;

int main(int argc, char **argv) {
   MiltonApp app(argc, argv, "Path Visualizer");
   
   app.getParams().forcePreview = true;
   
   if (!app.loadCommandlineParams())
      return 1;
   
   CanvasList &canvases = app.getCanvases();
   FOREACH(CanvasListIter, canvases, iter) {
      Canvas *canvas = *iter;
      
      if (canvas->supportsOpenGL()) {
         PathVisualizer *visualizer = 
            new PathVisualizer(dynamic_cast<OpenGLCanvas*>(canvas));
         visualizer->init();
         
         canvas->registerInteractionListener(visualizer);
      }
   }
   
   return app.exec();
}

