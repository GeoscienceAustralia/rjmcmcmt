@ECHO OFF

SET path=..\..\bin\x64\Release;%path%
SET path=C:\Microsoft_HPC_Pack_2012\Bin;%path%

REM run stand-alone
rem rjmcmcmt.exe rjmcmcmt.con

REM run with four MPI processes (and thus 4 Markov Chains)
mpiexec -np 4 rjmcmcmt.exe rjmcmcmt.con

PAUSE
