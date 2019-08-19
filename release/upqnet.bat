@echo off
rem Batch File to Update software of PQNet main program
rem Copyright (C) 2019 Boyuu Electric <http://www.boyuu.com.cn>
rem upqnet.bat V1.0.0 By Seapex

if {%1}=={} goto end1

rem up_pqied.exe %1 %2 > update_log/%1.txt
up_pqied.exe %1 %2
if %errorlevel%==0  goto succ
if %errorlevel%==1  goto notneed
echo Upgrade failed! > update_log/%1.txt
goto end

:succ
echo Upgrade succeeded! > update_log/%1.txt
goto end

:notneed
echo Don't need to upgrade! > update_log/%1.txt
goto end

:end1
@echo usage:	%0 ip [port]

:end
