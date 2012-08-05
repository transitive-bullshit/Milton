/* ---------------------------------------------- 
   file: LeastSquaresSolver.cpp
   auth: Travis Fischer
   acct: tfischer
   date: Spring 2009

   Wrapper around LinearSolver for efficiently solving equations of the form 
      AX=B
   in a least-squares sense, where A is sparse and not necessarily square, 
   and X and B are not necessarily vectors.
   
   A should be mxn, X should be nxk, and B should be mxk
   ---------------------------------------------- */

#include "LeastSquaresSolver.h"
#include "LinearSolver.h"

/**
 * Constructs a solver of the form: AX=B where
 * A is mxn, X is nxk, and B is mxk
 *
 * The dimensions cannot be changed after constructing the solver.
 */
LeastSquaresSolver::LeastSquaresSolver(unsigned m, unsigned n, unsigned k) 
   : m_m(m), m_n(n), m_k(k), 
     m_A(NULL), m_ATranspose(NULL), m_ATransposeA(NULL), 
     m_B(NULL), m_ATransposeB(NULL), 
     m_solver(NULL), 
     m_dirtyA(true), m_dirtyB(true)
{
   m_A = new SparseMatrix(m_m, m_n);
   m_B = new double[m_m * m_k];
}

/**
 * Constructs a solver of the form: AX=B where
 * A is mxn, X is nxk, and B is mxk
 *
 * The dimensions cannot be changed after constructing the solver.
 */
LeastSquaresSolver::LeastSquaresSolver(const SparseMatrix &A, unsigned k) 
   : m_m(A.getM()), m_n(A.getN()), m_k(k), 
     m_A(NULL), m_ATranspose(NULL), m_ATransposeA(NULL), 
     m_B(NULL), m_ATransposeB(NULL), 
     m_solver(NULL), 
     m_dirtyA(true), m_dirtyB(true)
{
   m_A = new SparseMatrix(A);
   m_B = new double[m_m * m_k];
}

LeastSquaresSolver::~LeastSquaresSolver() {
   safeDelete(m_solver);
   safeDelete(m_A);
   
   safeDelete(m_ATranspose);
   safeDelete(m_ATransposeA);
   
   safeDeleteArray(m_B);
   safeDeleteArray(m_ATransposeB);
}

/**
 * Solves the current sparse matrix.  Pass it the buffer to fill the 
 * results with.  The buffer should be allocated to be the size of X
 * (sizeof(double) * n * k).
 * 
 * Optional paramater 'str' will be prepended to any stdout output.
 *
 * Set the 'bailOnError' paramater to return after the first error 
 * encountered.
 *
 * Output will be row-major of the form:
 *    X = [ x11 x12 ... x1k ]
 *        [ x21 x22 ... x2k ]
 *        [       ...       ]
 *        [ xn1 xn2 ... xnk ]
 * 
 * @returns the number of systems successfully solved
 *   (will return k if completely successful)
 */
unsigned LeastSquaresSolver::solve(double *X, bool bailOnError) {
   ASSERT(m_m > 0 && m_n > 0 && m_k > 0);
   ASSERT(m_A);
   ASSERT(m_B);
   
   if (m_dirtyA || m_solver == NULL) {
      safeDelete(m_ATranspose);
      safeDelete(m_ATransposeA);
      
      m_ATranspose  = new SparseMatrix(m_A->getTranspose());
      m_ATransposeA = new SparseMatrix((*m_ATranspose) * (*m_A));
      
      if (m_solver == NULL)
         m_solver = new LinearSolver();
      
      m_solver->setA(*m_ATransposeA);
   }
   
   m_dirtyB |= m_dirtyA;
   if (m_dirtyB) {
      ASSERT(m_ATranspose != NULL);
      
      if (m_ATransposeB == NULL)
         m_ATransposeB = new double[m_n * m_k];
      
      m_ATranspose->multiply(m_ATransposeB, m_B, m_m, m_k);
   }
   
   // A should be nxn now
   // B should be nxk
   ASSERT(m_solver);
   ASSERT(m_ATransposeB);
   
   // m_k shouldn't be here, but we use it down below instead of allocating twice
   double *b = (double*) malloc(sizeof(double) * m_n * m_k);
   unsigned noSolved = 0;
   
   // solve each of the k Ax=b normal equations in AX=B
   for(unsigned i = m_k; i--;) {
      double *x = X + i * m_n;
      
      // Setup solution vector b by stacking the columns of B
      // Assumes linear system is independent of column in X and B
      // m_ATransposeB is currently nxk
      for(unsigned j = m_n; j--;)
         b[j] = m_ATransposeB[j * m_k + i];
      
      // Solve current Ax=b
      if (m_solver->solve(b, x)) {
         ++noSolved;
         continue;
      }
      
      if (bailOnError)
         break;
   }
   
   // Reorder output
   for(unsigned i = m_n; i--;)
      for(unsigned j = m_k; j--;)
         b[i * m_k + j] = X[i + j * m_n];
   
   memcpy(X, b, sizeof(double) * m_n * m_k);
   m_dirtyA = false;
   m_dirtyB = false;
   
   safeFree(b);
   return noSolved;
}

