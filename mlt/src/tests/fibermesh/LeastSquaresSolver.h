/* ---------------------------------------------- 
   file: LeastSquaresSolver.h
   auth: Travis Fischer
   acct: tfischer
   date: Spring 2009
   
   Wrapper around LinearSolver for efficiently solving equations of the form 
      AX=B
   in a least-squares sense, where A is sparse and not necessarily square, 
   and X and B are not necessarily vectors.
   
   A should be mxn, X should be nxk, and B should be mxk
   ---------------------------------------------- */

#ifndef LEAST_SQUARES_SOLVER_H_
#define LEAST_SQUARES_SOLVER_H_

#include "SparseMatrix.h"
#include <vector>
using namespace std;

class LinearSolver;

class LeastSquaresSolver {
   public:
      /**
       * Constructs a solver of the form: AX=B where
       * A is mxn, X is nxk, and B is mxk
       *
       * The dimensions cannot be changed after constructing the solver.
       */
      LeastSquaresSolver(unsigned m, unsigned n, unsigned k);
      
      /**
       * Constructs a solver of the form: AX=B where
       * A is mxn, X is nxk, and B is mxk
       *
       * The dimensions cannot be changed after constructing the solver.
       */
      LeastSquaresSolver(const SparseMatrix &A, unsigned k);
      
      virtual ~LeastSquaresSolver();
      
      /**
       * Solves the current sparse matrix.  Pass it the buffer to fill the 
       * results with.  The buffer should be allocated to be the size of X
       * (sizeof(double) * n * k).
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
      unsigned solve(double *x, bool bailOnError = false);
      
      /**
       * Accessor and mutator for SparseMatrix representing the mxn coefficient 
       * matrix, A.
       */
      inline SparseMatrix *getA() {
         m_dirtyA = true;
         return m_A;
      }
      
      inline void setA(const SparseMatrix &A) {
         ASSERT(m_A->getM() == A.getM() && m_A->getN() == A.getN());
         m_dirtyA = true;
         
         *m_A = A;
      }
      
      /**
       * Accessor and mutator for mxk known solution matrix, B
       */
      inline double *getB() {
         m_dirtyB = true;
         return m_B;
      }
      
      inline void setB(double *B) {
         m_dirtyB = true;
         
         memcpy(m_B, B, sizeof(double) * m_m * m_k);
      }
      
      /**
       * Accessors for values which may or may not exist, depending on A and B.
       * The point of these accessors is that these values will likely be 
       * reused in other parts of the computation, so only compute them once.
       */
      inline const SparseMatrix *getATranspose() const {
         return m_ATranspose;
      }
      
      inline const SparseMatrix *getATransposeA() const {
         return m_ATransposeA;
      }
      
      inline const double *getATransposeB() const {
         return m_ATransposeB;
      }
      
   protected:
      // A is mxn, X is nxk, B is mxk
      unsigned m_m, m_n, m_k;
      
      SparseMatrix *m_A, *m_ATranspose, *m_ATransposeA;
      double       *m_B, *m_ATransposeB;
      LinearSolver *m_solver;
      
      bool m_dirtyA;
      bool m_dirtyB;
};

#endif // LEAST_SQUARES_SOLVER_H_

