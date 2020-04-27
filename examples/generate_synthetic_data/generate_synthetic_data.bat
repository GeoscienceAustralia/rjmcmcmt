@ECHO OFF

REM DOS batch file to generate synthetic EDI data files for two different models with/without simulated synthetic noise added 

REM Set path to executables
SET path=..\..\bin\x64\Release;%path%

REM Usage: generate_synthetic_data.exe modelfile edifile nfrequencies frequency_low frequency_hi appres_relative_err appres_floor_err appphase_relative_err appphase_floor_err
REM	generate_synthetic_data.exe	program
REM	modelfile			input resistivity model file
REM	edifile			output EDI file
REM	frequency_low			lowest frequency (Hz)
REM	frequency_hi			highest frequency (Hz)
REM	appres_relative_err		stddev of apparent-resistivity relative error (fraction)
REM	appres_floor_err		stddev of apparent-resistivity absolute error (Ohm.m)
REM	appphase_relative_err		stddev of apparent-phase relative error (fraction)
REM	appphase_floor_err		stddev of apparent-phase absolute error (degrees)

generate_synthetic_data.exe Synthetic_Model_1.txt Synthetic_Model_1_no-noise.edi 54 1 10000 0    0   0    0
generate_synthetic_data.exe Synthetic_Model_1.txt Synthetic_Model_1_noise.edi    54 1 10000 0.05 0.1 0.02 1

generate_synthetic_data.exe Synthetic_Model_2.txt Synthetic_Model_2_no-noise.edi 54 1 10000 0    0   0    0
generate_synthetic_data.exe Synthetic_Model_2.txt Synthetic_Model_2_noise.edi    54 1 10000 0.05 0.1 0.02 1

REM Make a directory in which to the synthetic data example is run
mkdir ..\Synthetic\edifiles

REM Move the output EDI files to the synthetic example directory
move *.edi ..\Synthetic\edifiles\.

REM Copy the model files to synthetic example directory WITH a station name that matches the EDI file
copy Synthetic_Model_1.txt ..\Synthetic\edifiles\Synthetic_Model_1_no-noise.txt
copy Synthetic_Model_1.txt ..\Synthetic\edifiles\Synthetic_Model_1_noise.txt
copy Synthetic_Model_2.txt ..\Synthetic\edifiles\Synthetic_Model_2_no-noise.txt
copy Synthetic_Model_2.txt ..\Synthetic\edifiles\Synthetic_Model_2_noise.txt

REM  An input N-layer model should be in this form in a text file

REM  Model_Name
REM  thickness1 resistivity1
REM  thickness2 resistivity2
REM  ...
REM  ...
REM  Inf        resistivityN

PAUSE
