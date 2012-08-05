/**<!-------------------------------------------------------------------->
   @file   main.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   <!-------------------------------------------------------------------->**/

#include <MiltonApp.h>
#include <dlfcn.h>
using namespace milton;

int main(int argc, char **argv) {
   MiltonApp app(argc, argv);
   
   if (!app.loadCommandlineParams())
      return 1;
   
   return app.exec();
}

