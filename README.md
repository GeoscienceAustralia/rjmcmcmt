# RJMCMCMT Source Code Repository

## C++ code for transdimensional or reversible jump Markov chain Monte Carlo (rj-McMC) 1D inversion of magnetotelluric geophysical data.

- Author:	Ross C Brodie
- Language:	C++ (std >= c++17)

## Currently included programs
1. rjmcmcmt - stochastic 1D inversion program for magnetotelluric (MT) data
2. generate_synthetic_data - utility for generating synthetic MT data

## Documentation
- None to date

# Building

### Initialise the submodules
- Initialise and update the submodules 
	- See [submodules](submodules/README.md).

### Build the underlying rj-McMC engine library
- cd submodules/RJMCMC-1.0.11
- See the example script [submodules/RJMCMC-1.0.11/configure-ubuntu-win10.sh](submodules/RJMCMC-1.0.11/configure-ubuntu-win10.sh) used to build on the Ubuntu emulator on Windows 10.
- This script actually invokes the configure script [submodules/RJMCMC-1.0.11/configure](submodules/RJMCMC-1.0.11/configure) with suitable options.

### Building on Linux with CMake
- See the example [cmake_build_script_ubuntu.sh](cmake_build_script_ubuntu.sh)
- Note that here we must ensure that the LIBRJMCMC_ROOT variable points to the same path of the resultant installed (include and lib) that was created in the step above (ie. submodules/RJMCMC-1.0.11/install-ubuntu-win10)

### Building on Linux with makefiles (to be deprecated)
- cd makefiles
- See example scripts run_make_xxx.sh to setup for your compiler
	- set the C++ compiler (e.g., cxx=g++)
	- set the MPI C++ compiler (e.g., mpicxx=mpiCC)
	- set the C++ compiler flags (e.g. cxxflags='-std=c++17 -O3 -Wall')
	- set the executable directory (e.g., exedir='../bin/ubuntu-win10')
- execute run_make_xxx.sh

### Building on Windows
- You can build (compile) the programs with the free Microsoft Visual Studio Express 2019.
- Visual Studio solution and project files are supplied.
- Open submodules\RJMCMC-1.0.11\visualstudio\librjmcmc\librjmcmc.sln to compile the rj-McMC engine library
- Open visualstudio\rjmcmcmt\rjmcmcmt.sln to compile the inversion program.
- Open visualstudio\generate_synthetic_data\generate_synthetic_data.sln to compile the synthetic data generation program.

## Other software dependencies
1. cpp-utils
	- cpp-utils is a repository of C++ utility classes and functions that are used across this and several other other projects.
	- cpp-utils is included as a git submodule of this repository (see [submodules](submodules/README.md)).

2. rj-McMC
	- this is the underlying rj-McMC engine library written by Rhys Hawkins, Research School of Earth Science, Australian National University.
	- rj-McMC is included as a git submodule of this repository (see [submodules](submodules/README.md)).
	- it was originally download from http://www.iearth.org.au/codes/rj-MCMC/ and one small bug fix was applied, plus Visual Studio project and solution files have been added,

3. Message Passing Interface (MPI)
	- see https://www.mpi-forum.org
	- Both Windows and Linux users will need to install a suitable MPI for your system.
	- They can be recompiled using MPICH or other flouvours of MPI if required.

## License
This software is licensed under the GNU GPL v3 license. See the [license deed](LICENSE) for details.

## Contacts
**Ross Brodie**  
*Lead Developer*  
Geoscience Australia  
<ross.brodie@ga.gov.au>  
