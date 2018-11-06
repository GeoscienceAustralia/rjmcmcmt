@ECHO OFF

SET path=..\..\bin\x64\Release;%path%

REM Usage: generate_synthetic_data.exe modelfile edifile nfrequencies frequency_low frequency_hi appres_relative_err appres_floor_err appphase_relative_err appphase_floor_err

generate_synthetic_data.exe synthetic_model_1.txt synthetic_model_1_no-noise.edi 54 1 10000 0    0   0    0
generate_synthetic_data.exe synthetic_model_1.txt synthetic_model_1_noise.edi    54 1 10000 0.05 0.1 0.02 1

PAUSE

