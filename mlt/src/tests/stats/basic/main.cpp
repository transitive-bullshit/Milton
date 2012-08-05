#include <Stats.h>
#include <iostream>
using namespace std;
using namespace milton;

int main(int argc, char** argv) {
   Random::init();
   NormalSampler X;
   
   for(int i = 50; i--;) {
      //const double &x = X.sample().getValue<double>();
      const double &x = X.sample();
      
      cerr << x << endl;
   }
   
   return 0;
}

