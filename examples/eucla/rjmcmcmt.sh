#!/bin/bash

#This is an example PBS shell script for running on raijin.nci.org.au

#PBS -P r17
#PBS -q express
#PBS -l ncpus=16
#PBS -l mem=16GB
#PBS -l walltime=1:00:00
#PBS -l wd
#PBS -N rjmcmcmt
#PBS -o rjmcmcmt.out
#PBS -e rjmcmcmt.err
#PBS -j oe

#load the necessary module
module load rjmcmcmt/intel

#list the modules loaded as a check
module list

#Execute the program rjmcmcmt.exe in using MPI parallelism
mpirun rjmcmcmt.exe rjmcmcmt.con

