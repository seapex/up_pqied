@echo off
if {%1}=={} goto end1
rem @echo %0 %1 %2 %3
if "%1"=="app" goto app
if "%1"=="61850" goto 61850
if "%1"=="svx" goto svx
goto end1

:app
cd .sys\up_tmp
..\md5sum.exe * > ..\check.md5
cd ..\..
goto end

:61850
cd upfile\61850\%2
..\..\..\.sys\md5sum.exe %3 >> ..\..\..\.sys\check.md5
cd ..\..\..
goto end

:svx
cd upfile
..\.sys\md5sum.exe svx/* >> ..\.sys\check.md5
cd ..
goto end

:end1
@echo usage:	%0 type [oem] [directory]
@echo           type -- app, 61850, svx

:end
