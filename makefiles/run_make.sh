#!/bin/sh

#Script to load compiler modules and dependent software
export srcdir='../src'
export cpputilssrc='../submodules/cpp-utils/src'

#GNU compiler on raijin.nci.org.au
module load rjmcmcmt/gnu
export cxx=g++
export mpicxx=mpiCC
export cxxflags='-std=c++11 -O3 -Wall -fdiagnostics-color=always'
export exedir='../bin/raijin/gnu'

#Intel compiler on raijin.nci.org.au
#module load librjmcmc/intel
#export cxx=icpc
#export mpicxx=mpiCC
#export cxxflags='-std=c++11 -O3 -Wall -diag-disable remark'
#export exedir='../bin/raijin/intel'

mpiCC -showme
make -f rjmcmcmt.make $1
make -f generate_synthetic_data.make $1


