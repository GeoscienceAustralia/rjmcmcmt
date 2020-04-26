@echo OFF

SET path=..\..\..\bin\x64\Release;%path%

REM Usage: generate_synthetic_data.exe modelfile edifile nfrequencies frequency_low frequency_hi appres_relative_err appres_floor_err appphase_relative_err appphase_floor_err
generate_synthetic_data.exe Synthetic.txt Synthetic.edi 40 1 10000 0.05 0.01 0.02 0.5


pause

