
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

/*!
 @file ComputeSPMV_ref.cpp

 HPCG routine
 */
#if defined(HPCG_WITH_CUDA) | defined(HPCG_WITH_HIP)

#include "ComputeSPMV_ref.hpp"
#ifndef HPCG_NO_MPI
 #include "ExchangeHalo.hpp"
#endif

#ifndef HPCG_NO_OPENMP
 #include <omp.h>
#endif
#include <cassert>

#if defined(HPCH_WITH_CUDA)
 #include <cuda_runtime.h>
 #include <cusparse.h>
#elif defined(HPCG_WITH_HIP)
 #include <hip/hip_runtime_api.h>
 #include <rocblas.h>
#endif

#if defined(HPCG_DEBUG) & !defined(HPCG_NO_MPI)
 #include <mpi.h>
 #include "Utils_MPI.hpp"
 #include "hpgmp.hpp"
#endif


/*!
  Routine to compute matrix vector product y = Ax where:
  Precondition: First call exchange_externals to get off-processor values of x

  This is the reference SPMV implementation.  It CANNOT be modified for the
  purposes of this benchmark.

  @param[in]  A the known system matrix
  @param[in]  x the known vector
  @param[out] y the On exit contains the result: Ax.

  @return returns 0 upon success and non-zero otherwise

  @see ComputeSPMV
*/
template<class SparseMatrix_type, class Vector_type>
int ComputeSPMV_ref(const SparseMatrix_type & A, Vector_type & x, Vector_type & y) {

  assert(x.localLength>=A.localNumberOfColumns); // Test vector lengths
  assert(y.localLength>=A.localNumberOfRows);
  typedef typename SparseMatrix_type::scalar_type scalar_type;

  const scalar_type one  (1.0);
  const scalar_type zero (0.0);
  const local_int_t ncol = A.localNumberOfColumns;
  const global_int_t nnz = A.localNumberOfNonzeros;

  const local_int_t nrow = A.localNumberOfRows;
  scalar_type * const xv = x.values;
  scalar_type * const yv = y.values;

  scalar_type * const d_xv = x.d_values;
  scalar_type * const d_yv = y.d_values;

#ifndef HPCG_NO_MPI
  if (A.geom->size > 1) {
    #ifdef HPCG_WITH_CUDA
    // Copy local part of X to HOST CPU
    if (cudaSuccess != cudaMemcpy(xv, x.d_values, nrow*sizeof(scalar_type), cudaMemcpyDeviceToHost)) {
      printf( " Failed to memcpy d_y\n" );
    }
    #elif defined(HPCG_WITH_HIP)
    if (hipSuccess != hipMemcpy(xv, x.d_values, nrow*sizeof(scalar_type), hipMemcpyDeviceToHost)) {
      printf( " Failed to memcpy d_y\n" );
    }
    #endif

    ExchangeHalo(A, x);

    // copy non-local part of X to device (after Halo exchange)
    #if defined(HPCG_WITH_CUDA)
    if (cudaSuccess != cudaMemcpy(&d_xv[nrow], &xv[nrow], (ncol-nrow)*sizeof(scalar_type), cudaMemcpyHostToDevice)) {
      printf( " Failed to memcpy d_x\n" );
    }
    #elif defined(HPCG_WITH_HIP)
    if (hipSuccess != hipMemcpy(&d_xv[nrow], &xv[nrow], (ncol-nrow)*sizeof(scalar_type), hipMemcpyHostToDevice)) {
      printf( " Failed to memcpy d_x\n" );
    }
    #endif
  }
#endif

#if defined(HPCG_DEBUG)
  #ifndef HPCG_NO_OPENMP
  #pragma omp parallel for
  #endif
  for (local_int_t i=0; i< nrow; i++)  {
    scalar_type sum = 0.0;
    const scalar_type * const cur_vals = A.matrixValues[i];
    const local_int_t * const cur_inds = A.mtxIndL[i];
    const int cur_nnz = A.nonzerosInRow[i];

    for (int j=0; j< cur_nnz; j++)
      sum += cur_vals[j]*xv[cur_inds[j]];
    yv[i] = sum;
  }
#endif

  #if defined(HPCG_WITH_CUDA)
  if (std::is_same<scalar_type, double>::value) {
     if (CUSPARSE_STATUS_SUCCESS != cusparseDcsrmv(A.cusparseHandle, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                                   nrow, ncol, nnz,
                                                   (const double*)&one,  A.descrA,
                                                                         (double*)A.d_nzvals, A.d_row_ptr, A.d_col_idx,
                                                                         (double*)d_xv,
                                                   (const double*)&zero, (double*)d_yv)) {
       printf( " Failed cusparseDcsrmv\n" );
     }
  } else if (std::is_same<scalar_type, float>::value) {
     if (CUSPARSE_STATUS_SUCCESS != cusparseScsrmv(A.cusparseHandle, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                                   nrow, ncol, nnz,
                                                   (const float*)&one,  A.descrA,
                                                                        (float*)A.d_nzvals, A.d_row_ptr, A.d_col_idx,
                                                                        (float*)d_xv,
                                                   (const float*)&zero, (float*)d_yv)) {
       printf( " Failed cusparseScsrmv\n" );
     }
  }
  #elif defined(HPCG_WITH_HIP)
  rocsparse_datatype rocsparse_compute_type = rocsparse_datatype_f64_r;
  if (std::is_same<scalar_type, float>::value) {
    rocsparse_compute_type = rocsparse_datatype_f32_r;
  }
  size_t buffer_size = A.buffer_size_A;
  rocsparse_dnvec_descr vecX, vecY;
  rocsparse_create_dnvec_descr(&vecX, ncol, (void*)d_xv, rocsparse_compute_type);
  rocsparse_create_dnvec_descr(&vecY, nrow, (void*)d_yv, rocsparse_compute_type);
  if (rocsparse_status_success != rocsparse_spmv(A.rocsparseHandle, rocsparse_operation_none,
                                                 &one, A.descrA, vecX, &zero, vecY,
                                                 rocsparse_compute_type, rocsparse_spmv_alg_default,
                                                 &buffer_size, A.buffer_A)) {
    printf( " Failed rocsparse_spmv\n" );
  }
  #endif
  
  #ifdef HPCG_DEBUG
  scalar_type * tv = (scalar_type *)malloc(nrow * sizeof(scalar_type));
  #if defined(HPCG_WITH_CUDA)
  if (cudaSuccess != cudaMemcpy(tv, d_yv, nrow*sizeof(scalar_type), cudaMemcpyDeviceToHost)) {
    printf( " Failed to memcpy d_y\n" );
  }
  #elif defined(HPCG_WITH_HIP)
  if (hipSuccess != hipMemcpy(tv, d_yv, nrow*sizeof(scalar_type), hipMemcpyDeviceToHost)) {
    printf( " Failed to memcpy d_y\n" );
  }
  #endif

  scalar_type l_enorm = 0.0;
  scalar_type l_xnorm = 0.0;
  scalar_type l_ynorm = 0.0;
  scalar_type l_tnorm = 0.0;
  for (int j=0; j<nrow; j++) {
    l_enorm += (tv[j]-yv[j])*(tv[j]-yv[j]);
    l_xnorm += xv[j]*xv[j];
    l_ynorm += yv[j]*yv[j];
    l_tnorm += tv[j]*tv[j];
  }
  scalar_type enorm = 0.0;
  scalar_type xnorm = 0.0;
  scalar_type ynorm = 0.0;
  scalar_type tnorm = 0.0;
  #ifndef HPCG_NO_MPI
  MPI_Datatype MPI_SCALAR_TYPE = MpiTypeTraits<scalar_type>::getType ();
  MPI_Allreduce(&l_enorm, &enorm, 1, MPI_SCALAR_TYPE, MPI_SUM, A.comm);
  MPI_Allreduce(&l_xnorm, &xnorm, 1, MPI_SCALAR_TYPE, MPI_SUM, A.comm);
  MPI_Allreduce(&l_ynorm, &ynorm, 1, MPI_SCALAR_TYPE, MPI_SUM, A.comm);
  MPI_Allreduce(&l_tnorm, &tnorm, 1, MPI_SCALAR_TYPE, MPI_SUM, A.comm);
  #else
  enorm = l_enorm;
  xnorm = l_xnorm;
  ynorm = l_ynorm;
  tnorm = l_tnorm;
  #endif
  enorm = sqrt(enorm);
  xnorm = sqrt(xnorm);
  ynorm = sqrt(ynorm);
  tnorm = sqrt(tnorm);
  if (A.geom->rank == 0) {
    HPCG_fout << A.geom->rank << " : SpMV(" << nrow << " x " << ncol << "): error = " << enorm << " (x=" << xnorm << ", y=" << ynorm << ", t=" << tnorm << ")" << std::endl;
  }
  free(tv);
  #endif

  return 0;
}


/* --------------- *
 * specializations *
 * --------------- */

template
int ComputeSPMV_ref< SparseMatrix<double>, Vector<double> >(SparseMatrix<double> const&, Vector<double>&, Vector<double>&);

template
int ComputeSPMV_ref< SparseMatrix<float>, Vector<float> >(SparseMatrix<float> const&, Vector<float>&, Vector<float>&);

#endif