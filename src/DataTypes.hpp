
#ifndef HPGMP_DATA_TYPES_HPP
#define HPGMP_DATA_TYPES_HPP

#ifdef HPCG_WITH_KOKKOSKERNELS
#include <KokkosKernels_Handle.hpp>
#include <KokkosSparse_gauss_seidel.hpp>
typedef Kokkos::Experimental::half_t half_t;

#ifndef HPCG_NO_MPI
#include "mpi.h"
extern MPI_Datatype    HPGMP_MPI_HALF;
extern MPI_Op          MPI_SUM_HALF;
extern void HPGMP_FP16_SUM_FUNCTION(void* invec, void* inoutvec, int *len, MPI_Datatype *datatype);
#endif
#endif

#ifdef HPCG_WITH_CUDA
 #include <cuda_runtime.h>
 #include <cublas_v2.h>
 #include <cusparse.h>
#elif defined(HPCG_WITH_HIP)
 #include <hip/hip_runtime_api.h>
 #include <rocblas.h>
 #include <rocsparse.h>
#endif

//#ifdef HPCG_WITH_KOKKOSKERNELS
//#include <Kokkos_Half.hpp>
//#endif
#endif