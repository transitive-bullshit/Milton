// @author Travis Fischer (tfischer), Fall 2008
#include <Stats.h>
#include <iostream>
#include <assert.h>
using namespace std;

int main(int argc, char** argv) {
   Random::init();
   NormalSampler N;
   
   const unsigned long long noSamples = 100000;
   const double stepSize  = 0.01;
   const double max       = 6;
   const double min       = -max;
   
   const int Narray[] = { 5, 10, 15, 30, 100 };
   const int NNarray = 5;
   
   const int noFrequencies = (int)ceil((max - min) / stepSize);
   double *frequencies     = new double[NNarray * noFrequencies];
   
   memset(frequencies, 0, sizeof(double) * NNarray * noFrequencies);
   
   for(int i = 0; i < NNarray; ++i) {
      int n = Narray[i];
      
      double *Y = new double[n];
      double *X = new double[n];
      
      for(unsigned long long s = noSamples; s--;) {
         // generate n samples iid on N(0, 1)
         for(int j = n; j--;)
            Y[j] = N.sample();
         
         // normalize samples to n-sphere of radius sqrt(n)
         double sum = 0;
         for(int j = n; j--;)
            sum += Y[j] * Y[j];
         
         double coeff = sqrt(n) / sqrt(sum);
         
         for(int j = n; j--;) {
            X[j] = coeff * Y[j];
            
            assert(X[j] > min && X[j] < max);
            int index = (int)ceil((X[j] - min) / stepSize);
            
            ++frequencies[i * noFrequencies + index];
         }
      }
      
      const double coeff = 1.0 / (noSamples * stepSize * n);
      
      for(int f = noFrequencies; f--;)
         frequencies[i * noFrequencies + f] *= coeff;
      
      delete[] Y;
      delete[] X;
   }
   
   for(int i = 0; i < NNarray; ++i) {
      int n = Narray[i];
      
      cout << n << ": " << endl;
      for(int f = noFrequencies; f--;) {
         cout << frequencies[i * noFrequencies + f] << " ";
      }
      
      cout << endl << endl;
   }
   
   cout << "Range: " << endl;
   for(int f = 0; f < noFrequencies; ++f) {
      const double rangeMin = (min + stepSize * f);
      
      cout << rangeMin << " ";
   }
   
   cout << endl << endl;
   
   cout << "StdNormal: " << endl;
   for(int f = 0; f < noFrequencies; ++f) {
      const double rangeMin = (min + stepSize * f);
      const double val = (1.0/sqrt(2*M_PI))*exp(-(rangeMin*rangeMin)/2);
      
      cout << val << " ";
   }
   
   cout << endl << endl;
   
   delete[] frequencies;
   return 0;
}

