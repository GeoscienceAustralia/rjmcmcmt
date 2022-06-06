#!/bin/sh

#Script to load compiler modules and dependent software
export exedir='../bin/ubuntu-win10'
export srcdir='../src'
export cpputilssrc='../submodules/cpp-utils/src'
export LIBRJMCMC_ROOT=$PWD/../submodules/RJMCMC-1.0.11/install-ubuntu-win10

#GNU compiler
export cxx=g++
export mpicxx=mpiCC
export cxxflags='-std=c++17 -O3 -fdiagnostics-color=always -Wall -Wno-format-security -Wno-unused-result'

mpiCC -showme
make -f rjmcmcmt.make $1
make -f generate_synthetic_data.make $1

