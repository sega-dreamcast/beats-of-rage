@echo off
title Beats of Rage Rumble Edition for Sega Dreamcast: Clean

set SCRIPT_PATH=%~dp0
set SCRIPT_PATH=%SCRIPT_PATH:~0,-1%
set MAKE=%SCRIPT_PATH%\sdk\bin\make.exe
cd src\bor

%MAKE% clean
pause
