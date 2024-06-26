# -*- Makefile -*-

# by default, "arch" is unknown, should be specified in the command line
arch = UNKNOWN

setup_file = setup/Make.$(arch)
include $(setup_file)

HPGMP_DEPS = src/ComputeResidual.o \
         src/CheckProblem.o \
         src/GenerateGeometry.o \
         src/ExchangeHalo.o src/ExchangeHalo_ref.o src/ExchangeHalo_gpu.o \
	 src/OptimizeProblem.o src/ReadHpgmpDat.o src/ReportResults.o \
	 src/SetupHalo.o src/SetupHalo_ref.o src/WriteProblem.o \
         src/YAML_Doc.o src/YAML_Element.o \
         src/ComputeDotProduct.o src/ComputeDotProduct_ref.o \
         src/ComputeDotProduct_blas.o src/ComputeDotProduct_gpu.o \
         src/finalize.o src/init.o src/mytimer.o \
         src/ComputeSPMV.o src/ComputeSPMV_ref.o \
         src/ComputeSPMV_gpu.o \
	 src/ComputeSYMGS.o src/ComputeSYMGS_ref.o \
         src/ComputeWAXPBY.o src/ComputeWAXPBY_ref.o \
         src/ComputeMG_ref.o src/ComputeMG.o \
         src/ComputeProlongation_ref.o src/ComputeRestriction_ref.o \
         src/ComputeOptimalShapeXYZ.o src/MixedBaseCounter.o src/CheckAspectRatio.o src/OutputFile.o \
         \
         src/TestGMRES.o src/BenchGMRES.o src/ValidGMRES.o \
         src/ComputeTRSM.o \
	 src/ComputeGEMV.o src/ComputeGEMVT.o \
         src/ComputeGEMV.o src/ComputeGEMV_ref.o src/ComputeGEMV_blas.o src/ComputeGEMV_gpu.o \
         src/ComputeGEMVT.o src/ComputeGEMVT_ref.o src/ComputeGEMVT_blas.o src/ComputeGEMVT_gpu.o \
         src/ComputeGEMMT.o src/ComputeGEMMT_ref.o src/ComputeGEMMT_gpu.o \
         src/GMRES.o src/GMRES_IR.o \
         src/ComputeGS_Forward.o src/ComputeGS_Forward_ref.o src/ComputeGS_Forward_gpu.o \
         src/SetupProblem.o src/SetupMatrix.o \
         src/GenerateNonsymProblem.o src/GenerateNonsymProblem_v1_ref.o \
         src/GenerateNonsymCoarseProblem.o 

bin/xhpgmp: src/main_hpgmp.o $(HPGMP_DEPS)
	$(LINKER) $(LINKFLAGS) src/main_hpgmp.o $(HPGMP_DEPS) -o bin/xhpgmp $(HPGMP_LIBS)

bin/xhpgmp_time: src/main_time.o $(HPGMP_DEPS)
	$(LINKER) $(LINKFLAGS) src/main_time.o $(HPGMP_DEPS) -o bin/xhpgmp_time $(HPGMP_LIBS)

clean:
	rm -f $(HPGMP_DEPS) \
	bin/xhpgmp src/main_hpgmp.o \
	bin/xhpgmp_time src/main_time.o

.PHONY: clean

