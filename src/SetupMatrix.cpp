
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
 @file GenerateProblem.cpp

 HPGMP routine
 */

#include "SetupMatrix.hpp"


/*!
  Routine to generate a sparse matrix, right hand side, initial guess, and exact solution.

  @param[in]  A        The generated system matrix
  @param[inout] b      The newly allocated and generated right hand side vector (if b!=0 on entry)
  @param[inout] x      The newly allocated solution vector with entries set to 0.0 (if x!=0 on entry)
  @param[inout] xexact The newly allocated solution vector with entries set to the exact solution (if the xexact!=0 non-zero on entry)

  @see GenerateGeometry
*/

template<class SparseMatrix_type, class GMRESData_type, class Vector_type>
void SetupMatrix(int numberOfMgLevels, SparseMatrix_type & A, Geometry * geom, GMRESData_type & data,
                 Vector_type * b, Vector_type * x, Vector_type * xexact, bool init_vect, comm_type comm) {

  InitializeSparseMatrix(A, geom, comm);

  GenerateNonsymProblem(A, b, x, xexact, init_vect);
  SetupHalo(A); //TODO: This is currently called in main... Should it really be called in both places?  Which one? 

  A.localNumberOfMGNonzeros = A.localNumberOfNonzeros;
  A.totalNumberOfMGNonzeros = A.totalNumberOfNonzeros;

  SparseMatrix_type * curLevelMatrix = &A;
  for (int level = 1; level< numberOfMgLevels; ++level) {
    GenerateNonsymCoarseProblem(*curLevelMatrix);
    A.localNumberOfMGNonzeros += curLevelMatrix->Ac->localNumberOfNonzeros;
    A.totalNumberOfMGNonzeros += curLevelMatrix->Ac->totalNumberOfNonzeros;
    curLevelMatrix = curLevelMatrix->Ac; // Make the just-constructed coarse grid the next level
  }

//TODO: Reinstate "CheckProblem" for nonsymm version. 
/*  #ifndef NONSYMM_PROBLEM
  curLevelMatrix = &A;
  Vector_type * curb = b;
  Vector_type * curx = x;
  Vector_type * curxexact = xexact;
  for (int level = 0; level< numberOfMgLevels; ++level) {
     CheckProblem(*curLevelMatrix, curb, curx, curxexact);
     curLevelMatrix = curLevelMatrix->Ac; // Make the nextcoarse grid the next level
     curb = 0; // No vectors after the top level
     curx = 0;
     curxexact = 0;
  }
  #endif */

  InitializeSparseGMRESData(A, data);
}

/* --------------- *
 * specializations *
 * --------------- */

// uniform
template
void SetupMatrix< SparseMatrix<double>, GMRESData<double>, class Vector<double> >
 (int numberOfMgLevels, SparseMatrix<double> & A, Geometry * geom, GMRESData<double> & data, Vector<double> * b, Vector<double> * x, Vector<double> * xexact,
  bool init_vect, comm_type comm);

template
void SetupMatrix< SparseMatrix<float>, GMRESData<float>, class Vector<float> >
 (int numberOfMgLevels, SparseMatrix<float> & A, Geometry * geom, GMRESData<float> & data, Vector<float> * b, Vector<float> * x, Vector<float> * xexact,
  bool init_vect, comm_type comm);


// mixed
template
void SetupMatrix< SparseMatrix<float>, GMRESData<float>, class Vector<double> >
 (int numberOfMgLevels, SparseMatrix<float> & A, Geometry * geom, GMRESData<float> & data, Vector<double> * b, Vector<double> * x, Vector<double> * xexact,
  bool init_vect, comm_type comm);

