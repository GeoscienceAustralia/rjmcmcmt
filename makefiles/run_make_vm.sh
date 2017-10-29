#!/bin/sh

#Script to load compiler modules and dependent software
export exedir='../bin'
export srcdir='../src'
export cpputilssrc='../submodules/cpp-utils/src'
export LIBRJMCMC_ROOT='/home/niket/repos/librjmcmc'

#GNU compiler
export cxx=g++
export mpicxx=mpiCC
export cxxflags='-std=c++11 -O3 -Wall -fdiagnostics-color=always'

mpiCC -showme
make -f rjmcmcmt.make $1
make -f generate_synthetic_data.make $1


