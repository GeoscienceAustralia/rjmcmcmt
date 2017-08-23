@ECHO OFF
SET path=Z:\code\repos\rjmcmcmt\bin\x64\Release;%path%
SET path=C:\Microsoft_HPC_Pack_2012\Bin;%path%

rjmcmcmt.exe rjmcmcmt.con
REM mpiexec -np 2 rjmcmcmt.exe rjmcmcmt.con

PAUSE
