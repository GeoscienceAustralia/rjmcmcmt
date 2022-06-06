#!/bin/sh
# BUILD_DIR is a temporary directory for building (compiling and linking)

# Set this to the pre-built path of the RJMCMC-1.0.11 engine library
export LIBRJMCMC_ROOT=$PWD/submodules/RJMCMC-1.0.11/install-ubuntu-win10
export BUILD_DIR=$PWD/build-ubuntu-win10
export INSTALL_DIR=$PWD/install-ubuntu-win10

mkdir $BUILD_DIR
cd $BUILD_DIR

#eg for the GNU compilers
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target 
cmake --install . --prefix $INSTALL_DIR

