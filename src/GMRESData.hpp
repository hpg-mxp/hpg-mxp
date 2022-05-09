
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
 @file CGData.hpp

 HPCG data structure
 */

#ifndef GMRESDATA_HPP
#define GMRESDATA_HPP

#include "SparseMatrix.hpp"
#include "Vector.hpp"

template <class SC>
class GMRESData {
public:
  Vector<SC> r; //!< pointer to residual vector
  Vector<SC> z; //!< pointer to preconditioned residual vector
  Vector<SC> p; //!< pointer to direction vector
  Vector<SC> w; //!< pointer to workspace
  Vector<SC> Ap; //!< pointer to Krylov vector
};

/*!
 Constructor for the data structure of GMRES vectors.

 @param[in]  A    the data structure that describes the problem matrix and its structure
 @param[out] data the data structure for GMRES  vectors that will be allocated to get it ready for use in GMRES iterations
 */
template <class SparseMatrix_type, class GMRESData_type>
inline void InitializeSparseGMRESData(SparseMatrix_type & A, GMRESData_type & data) {
  local_int_t nrow = A.localNumberOfRows;
  local_int_t ncol = A.localNumberOfColumns;
  comm_type comm = A.comm;

  InitializeVector(data.r,  nrow, comm);
  InitializeVector(data.z,  ncol, comm);
  InitializeVector(data.p,  ncol, comm);
  InitializeVector(data.w,  nrow, comm);
  InitializeVector(data.Ap, nrow, comm);
  return;
}

/*!
 Destructor for the GMRES vectors data.

 @param[inout] data the GMRES vectors data structure whose storage is deallocated
 */
template <class GMRESData_type>
inline void DeleteGMRESData(GMRESData_type & data) {

  DeleteVector (data.r);
  DeleteVector (data.z);
  DeleteVector (data.p);
  DeleteVector (data.w);
  DeleteVector (data.Ap);
  return;
}



template<class SC>
class TestGMRESData {
public:
  int restart_length;     //!< restart length
  SC tolerance;           //!< tolerance = reference residual norm 
  double runningTime;     //!<
  double minOfficialTime; //!<

  // from validation step
  int refNumIters;       //!< number of reference iterations
  int optNumIters;       //!< number of optimized iterations
  int validation_nprocs; //!<
  double refResNorm0;
  double refResNorm;
  double optResNorm0;
  double optResNorm;

  double SetupTime;
  double OptimizeTime;
  double SpmvMgTime;

  // from benchmark step
  int numOfCalls;       //!< number of calls
  int maxNumIters;      //!< 
  double refTotalFlops; //
  double refTotalTime;  //
  double optTotalFlops; //
  double optTotalTime;  //
  double *flops;        //!< total, dot, axpy, ortho, spmv, reduce, precond
  double *times;        //!< total, dot, axpy, ortho, spmv, reduce, precond
};

#endif // CGDATA_HPP
