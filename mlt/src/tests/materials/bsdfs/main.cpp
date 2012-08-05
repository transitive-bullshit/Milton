/**<!-------------------------------------------------------------------->
   @file   main.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   <!-------------------------------------------------------------------->**/

#include "BSDFVisualizer.h"
#include <MiltonApp.h>

int main(int argc, char **argv) {
   MiltonApp app(argc, argv, "BSDF Visualizer");
   
   OpenGLCanvas *canvas = new OpenGLCanvas();
   canvas->registerInteractionListener(new BSDFVisualizer(canvas));
   
   app.addCanvas(canvas);
   
   return app.exec();
}

