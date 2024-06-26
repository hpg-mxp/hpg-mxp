
//@HEADER
// ***************************************************
//
// HPGMP: High Performance Generalized minimal residual
//        - Mixed-Precision
//
// Contact:
// Ichitaro Yamazaki         (iyamaza@sandia.gov)
// Sivasankaran Rajamanickam (srajama@sandia.gov)
// Piotr Luszczek            (luszczek@eecs.utk.edu)
// Jack Dongarra             (dongarra@eecs.utk.edu)
//
// ***************************************************
//@HEADER

/*!
 @file ComputeMG.cpp

 HPGMP routine
 */

#include "ComputeMG.hpp"
#include "ComputeMG_ref.hpp"

/*!
  @param[in] A the known system matrix
  @param[in] r the input vector
  @param[inout] x On exit contains the result of the multigrid V-cycle with r as the RHS, x is the approximation to Ax = r.

  @return returns 0 upon success and non-zero otherwise

  @see ComputeMG_ref
*/
template<class SparseMatrix_type, class Vector_type>
int ComputeMG(const SparseMatrix_type & A, const Vector_type & r, Vector_type & x, bool symmetric) {

  // This line and the next two lines should be removed and your version of ComputeSYMGS should be used.
  A.isMgOptimized = false;
  return ComputeMG_ref(A, r, x, symmetric);
}


/* --------------- *
 * specializations *
 * --------------- */

template
int ComputeMG< SparseMatrix<double>, Vector<double> >(SparseMatrix<double> const&, Vector<double> const&, Vector<double>&, bool);

template
int ComputeMG< SparseMatrix<float>, Vector<float> >(SparseMatrix<float> const&, Vector<float> const&, Vector<float>&, bool);

