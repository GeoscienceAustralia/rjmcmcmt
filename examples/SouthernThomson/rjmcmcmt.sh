#!/bin/bash

#This is an example PBS shell script for running on raijin.nci.org.au

#PBS -P r17
#PBS -q express
#PBS -l ncpus=32
#PBS -l mem=32GB
#PBS -l walltime=00:20:00
#PBS -N rjmcmcmt
#PBS -o rjmcmcmt.out
#PBS -e rjmcmcmt.err
#PBS -j oe
#PBS -l wd


module load rjmcmcmt/intel

mpirun -n 32 rjmcmcmt.exe rjmcmcmt.con

