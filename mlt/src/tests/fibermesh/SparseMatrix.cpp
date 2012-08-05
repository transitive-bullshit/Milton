/* ---------------------------------------------- 
   file: SparseMatrix.cpp
   auth: Travis Fischer
   acct: tfischer
   date: Spring 2008

   Provides basic functionality for a sparse matrix of 
   arbitrary dimensions
   ---------------------------------------------- */

#include "SparseMatrix.h"
#include <milton.h>

// Returns the transpose of this matrix
const SparseMatrix SparseMatrix::getTranspose() const {
   SparseArray m;
   
   SparseColArrayConstIterator j;
   SparseArrayConstIterator i;
   
   for(i = m_elements.begin(); i != m_elements.end(); ++i)
      for(j = i->second.begin(); j != i->second.end(); ++j)
         m[j->first][i->first] = j->second;
   
   // note m and n are switched
   return SparseMatrix(m_n, m_m, m);
}

// Returns the Frobenius Norm of this matrix
double SparseMatrix::getFrobeniusNorm() {
   double squaredNorm = 0.0;
   
   SparseColArrayConstIterator j;
   SparseArrayConstIterator i;
   
   for(i = m_elements.begin(); i != m_elements.end(); ++i)
      for(j = i->second.begin(); j != i->second.end(); ++j)
         squaredNorm += j->second * j->second;
   
   return sqrt(squaredNorm);
}

// Multiplies this MxN SparseMatrix by the dense mxn matrix 'rhs' and stores 
// the dense result in 'out' which should be preallocated to hold Mxn doubles
void SparseMatrix::multiply(double *out, const double *rhs, int m, int n) {
   ASSERT(m_n == m); // Ensure dimensions match up
   
   memset(out, 0, sizeof(double) * m_m * n);
   SparseColArrayConstIterator j;
   SparseArrayConstIterator i;
   
   for(i = m_elements.begin(); i != m_elements.end(); ++i) {
      const int row = i->first;
      
      for(j = i->second.begin(); j != i->second.end(); ++j) {
         const int col = j->first;
         
         for(int k = n; k--;)
            out[row * n + k] += j->second * rhs[col * n + k];
      }
   }
}

// Multiplies this MxN SparseMatrix by the dense mxn matrix 'rhs' and stores 
// the result in the sparse Mxn matrix 'out'
void SparseMatrix::multiply(SparseMatrix &out, const double *rhs, int m, int n) {
   ASSERT(m_n == m); // Ensure dimensions match up
   ASSERT(out.getM() == m_m && out.getN() == n);

   SparseColArrayConstIterator j;
   SparseArrayConstIterator i;

   for(i = m_elements.begin(); i != m_elements.end(); ++i) {
      const int row = i->first;

      for(j = i->second.begin(); j != i->second.end(); ++j) {
         const int col = j->first;

         for(int k = n; k--;) {
            double value = j->second * rhs[col * n + k];

            if (value != 0)
               out.setValue(row, k, out.getValue(row, k) + value);
         }
      }
   }
}

bool SparseMatrix::save(const std::string &filename) const {
   RgbaImage *output = new RgbaImage(m_n, m_m);
   Rgba32 *data      = output->getData();
   
   SparseColArrayConstIterator j;
   SparseArrayConstIterator i;
   
   for(i = m_elements.begin(); i != m_elements.end(); ++i) {
      for(j = i->second.begin(); j != i->second.end(); ++j) {
         unsigned val = 255;
         
         data[i->first * m_n + j->first].r = val;
         data[i->first * m_n + j->first].g = val;
         data[i->first * m_n + j->first].b = val;
      }
   }
   
   bool success = output->save(filename.c_str());
   safeDelete(output);
   
   return success;
}

