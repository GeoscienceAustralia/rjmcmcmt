@echo OFF

set path=C:\Users\u61612\AppData\Local\Programs\Python\Python36;%path%

set PYTHONPATH=Z:\code\repos\rjmcmcmt\python;%PYTHONPATH%
echo %PYTHONPATH%
where python

python Z:\code\repos\rjmcmcmt\python\PlotResults.py Z:\code\repos\rjmcmcmt\examples\eucla\output\A_AK2\ Z:\code\repos\rjmcmcmt\examples\eucla\output\A_AK2\results.pdf

pause

