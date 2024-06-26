
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

#ifndef GMRES_IR_HPP
#define GMRES_IR_HPP

#include "SparseMatrix.hpp"
#include "MultiVector.hpp"
#include "Vector.hpp"
#include "SerialDenseMatrix.hpp"
#include "GMRESData.hpp"

template<class SparseMatrix_type, class SparseMatrix_type2, class GMRESData_type, class GMRESData_type2, class Vector_type, class TestGMRESData_type>
int GMRES_IR(const SparseMatrix_type & A, const SparseMatrix_type2 & A_lo,
             GMRESData_type & data, GMRESData_type2 & data_lo, const Vector_type & b_hi, Vector_type & x_hi,
             const int restart_length, const int max_iter, const typename SparseMatrix_type::scalar_type tolerance,
             int & niters, typename SparseMatrix_type::scalar_type & normr, typename SparseMatrix_type::scalar_type & normr0,
             bool doPreconditioning, bool verbose, TestGMRESData_type& test_data);

// this function will compute the Conjugate Gradient iterations.
// geom - Domain and processor topology information
// A - Matrix
// b - constant
// x - used for return value
// max_iter - how many times we iterate
// tolerance - Stopping tolerance for preconditioned iterations.
// niters - number of iterations performed
// normr - computed residual norm
// normr0 - Original residual
// times - array of timing information
// doPreconditioning - bool to specify whether or not symmetric GS will be applied.

#endif  // GMRES_IR_HPP
