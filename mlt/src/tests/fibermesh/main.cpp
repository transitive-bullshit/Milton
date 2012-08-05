/**<!-------------------------------------------------------------------->
   @file   main.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   <!-------------------------------------------------------------------->**/

#include "Frontend.h"
#include <MiltonApp.h>
#include <Qt/QtOpenGL>

#include "LeastSquaresSolver.h"

int main(int argc, char **argv) {
   MiltonApp app(argc, argv, "Fibermesh");
   
   {
      OpenGLCanvas *canvas = new OpenGLCanvas(false);
      canvas->registerInteractionListener(new Frontend(canvas));
      
      app.addCanvas(canvas);
   }
   
   return app.exec();
   
#if 0
   const unsigned n = 5, k = 3;
   
   SparseMatrix A(SparseMatrix::identity(n));
   double *b = new double[n * k];
   double *x = new double[n * k];
   
   for(unsigned i = n * k; i--;)
      b[i] = i;
   
   LeastSquaresSolver solver(A, k);
   unsigned no = 0;
   
   if ((no = solver.solve(x)) < k)
      cerr << "error: ";
   
   cerr << no << " systems solved out of " << k << endl;
#endif
   
   return 0;
}

