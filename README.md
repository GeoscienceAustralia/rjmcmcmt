# RJMCMCMT Source Code Repository

## C++ code for transdimensional or reversible jump Markov chain Monte Carlo (rj-McMC) 1D inversion of magnetotelluric geophysical data.

- Author:	Ross C Brodie
- Language:	C++

## Currently included programs
1. rjmcmcmt - stochastic 1D inversion program for magnetotelluric (MT) data
2. generate_synthetic_data - utility for generating synthetic MT data

## Documentation
- None to date

## Building on Linux
- cd makefiles
- edit the file run_make.sh to setup for your compiler
	- set the C++ compiler (e.g., cxx=g++)
	- set the MPI C++ compiler (e.g., mpicxx=mpiCC)
	- set the C++ compiler flags (e.g. cxxflags='-std=c++11 -O3 -Wall')
	- set the executable directory (e.g., exedir='../bin/raijin/gnu')
- execute run_make.sh

## Building on Windows
- You can build the programs with the free Microsoft Visual Studio 2013 Express.
- Visual Studio solution and project files are supplied.
- Open vs2013\rjmcmcmt\rjmcmcmt.sln to compile the inversion program.
- Open vs2013\generate_synthetic_data\generate_synthetic_data.sln to compile the synthetic data generation program.

## Other software dependencies
1. cpp-utils
	- cpp-utils is a repository of C++ utility classes and functions that are used across this and several other other projects.
	- cpp-utils is included as a git submodule of this repository (see [submodules](submodules/README.md)).
	- only required if you are compiling the code.
	- not required if you are just going to use the precompiled executables.

2. rj-McMC
	- this is a rj-McMC library written by Rhys Hawkins, Research School of Earth Science, Australian National University.
	- download from http://www.iearth.org.au/codes/rj-MCMC/

3. Message Passing Interface (MPI)
	- see https://www.mpi-forum.org
	- Both Windows and Linux users will need to install a suitable MPI for your system.
	- The pre-compiled Windows inversion programs require Microsoft HPC Pack 2012 because thats the flavour of MPI they have been compiled and linked with.
	- They can be recompiled using MPICH or other flouvours of MPI if required.

