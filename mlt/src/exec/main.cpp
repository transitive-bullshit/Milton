/**<!-------------------------------------------------------------------->
   @file   main.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   <!-------------------------------------------------------------------->**/

#include <MiltonApp.h>
#include <Visualization.h>
#include <QtOpenGL/QtOpenGL>
#include <QtGui/QtGui>

using namespace milton;

int main(int argc, char **argv) {
   MiltonApp app(argc, argv);
   
   if (!app.loadCommandlineParams())
      return 1;
   
   CanvasList &canvases = app.getCanvases();
   FOREACH(CanvasListIter, canvases, iter) {
      Canvas *canvas = *iter;
      
      canvas->registerInteractionListener(new IntersectDebugTester(canvas));
   }
   
   return app.exec();
}

