// @author Travis Fischer (tfischer), Fall 2008
#include <Stats.h>
#include <iostream>
#include <assert.h>
using namespace std;

int main(int argc, char** argv) {
   Random::init();
   
   const int N = 1000;
   
   double meanArray[] = { -10, -5, 0, 5, 10 };
   const unsigned int noMeans = 5;
   
   for(unsigned int i = 0; i < noMeans; ++i) {
      const double mean = meanArray[i];
      
      NormalSampler normal(mean, 1);
      
      cout << "Mean: " << mean << endl;
      cout << "[ ";
      for(int j = 0; j < N; ++j) {
         cout << "[ ";
         for(int k = 0; k < N; ++k) {
            const double w = ((j == k) - normal.sample() / N);
            cout << w << " ";
         }
         cout << " ]";
         if (j < N - 1)
            cout << endl;
      }
      cout << " ]" << endl;
      cout << endl;
   }
   
   return 0;
}

