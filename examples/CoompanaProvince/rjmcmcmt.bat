@ECHO OFF

REM If using MPI make sure its mpiexec is in the path
SET path=C:\Program Files\Microsoft MPI\Bin;%path%

REM Make sure rjmcmcmt.exe is in the path
SET path=..\..\bin\x64\Release;%path%

mpiexec -np 6 rjmcmcmt.exe rjmcmcmt.con

PAUSE
