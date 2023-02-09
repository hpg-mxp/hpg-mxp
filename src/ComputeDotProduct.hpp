
//@HEADER
// ***************************************************
//
// HPCG: High Performance Conjugate Gradient Benchmark
//
// Contact:
// Michael A. Heroux ( maherou@sandia.gov)
// Jack Dongarra     (dongarra@eecs.utk.edu)
// Piotr Luszczek    (luszczek@eecs.utk.edu)
//
// ***************************************************
//@HEADER

#ifndef COMPUTEDOTPRODUCT_HPP
#define COMPUTEDOTPRODUCT_HPP
#include "Vector.hpp"

template<class Vector_type, class scalar_type = typename Vector_type::scalar_type>
int ComputeDotProduct(const local_int_t n, const Vector_type & x, const Vector_type & y,
                      scalar_type & result, double & time_allreduce, bool & isOptimized);

#endif // COMPUTEDOTPRODUCT_HPP
