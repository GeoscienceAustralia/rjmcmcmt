#!/bin/bash

#Script to generate synthetic EDI data files for two different models with/without simulated synthetic noise added 

module load rjmcmcmt/intel
module show rjmcmcmt/intel
which generate_synthetic_data.exe

#Usage: generate_synthetic_data.exe modelfile edifile nfrequencies frequency_low frequency_hi appres_relative_err appres_floor_err appphase_relative_err appphase_floor_err

#generate_synthetic_data.exe	program
#modelfile			input resistivity model file
#edifile			output EDI file
#nfrequencies			number of frequencies
#frequency_low			lowest frequency (Hz)
#frequency_hi			highest frequency (Hz)
#appres_relative_err		stddev of apparent-resistivity relative error (fraction)
#appres_floor_err		stddev of apparent-resistivity absolute error (Ohm.m)
#appphase_relative_err		stddev of apparent-phase relative error (fraction)
#appphase_floor_err		stddev of apparent-phase absolute error (degrees)

generate_synthetic_data.exe Synthetic_Model_1.txt Synthetic_Model_1_no-noise.edi 54 1 10000 0    0   0    0
generate_synthetic_data.exe Synthetic_Model_1.txt Synthetic_Model_1_noise.edi    54 1 10000 0.05 0.1 0.02 1

generate_synthetic_data.exe Synthetic_Model_2.txt Synthetic_Model_2_no-noise.edi 54 1 10000 0    0   0    0
generate_synthetic_data.exe Synthetic_Model_2.txt Synthetic_Model_2_noise.edi    54 1 10000 0.05 0.1 0.02 1

#Make a directory in which to the synthetic data example is run
mkdir -p ../Synthetic/edifiles

#Move the output EDI files to the synthetic example directory
mv *.edi ../Synthetic/edifiles/.

#Copy the model files to synthetic example directory WITH a station name that matches the EDI file
cp Synthetic_Model_1.txt ../Synthetic/edifiles/Synthetic_Model_1_no-noise.txt
cp Synthetic_Model_1.txt ../Synthetic/edifiles/Synthetic_Model_1_noise.txt
cp Synthetic_Model_2.txt ../Synthetic/edifiles/Synthetic_Model_2_no-noise.txt
cp Synthetic_Model_2.txt ../Synthetic/edifiles/Synthetic_Model_2_noise.txt

#An input N-layer model should be in this form in a text file

# Model_Name
# thickness1 resistivity1
# thickness2 resistivity2
# ...
# ...
# Inf        resistivityN
