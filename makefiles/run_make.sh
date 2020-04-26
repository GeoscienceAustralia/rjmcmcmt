#!/bin/sh

#Script to load compiler modules and dependent software
export srcdir='../src'
export cpputilssrc='../submodules/cpp-utils/src'

#GNU compiler on gadi.nci.org.au
module load rjmcmcmt/gnu
export cxx=g++
export mpicxx=mpiCC
export cxxflags='-std=c++11 -O3 -Wall -fdiagnostics-color=always'
export exedir='../bin/gadi/gnu'

#Intel compiler on gadi.nci.org.au
#module load rjmcmcmt/intel
#export cxx=icpc
#export mpicxx=mpiCC
#export cxxflags='-std=c++11 -O3 -Wall -diag-disable remark'
#export exedir='../bin/gadi/intel'

mpiCC -showme
make -f rjmcmcmt.make $1
make -f generate_synthetic_data.make $1


