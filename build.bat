@echo off
title Beats of Rage Rumble Edition for Sega Dreamcast: Build

set SCRIPT_PATH=%~dp0
set SCRIPT_PATH=%SCRIPT_PATH:~0,-1%
set MAKE=%SCRIPT_PATH%\sdk\bin\make.exe
set BORPAK=%SCRIPT_PATH%\sdk\opt\borpak.exe
set CDROOT_PATH=%SCRIPT_PATH%\cd_root
cd src\bor

if not exist %CDROOT_PATH%\IP.BIN goto err_bootstrap
if not exist %CDROOT_PATH%\1ST_READ.BIN goto err_bootfile

:make_ntsc
set NTSC=1
echo Building NTSC binary...
%MAKE% clean
%MAKE%
move 1ST_READ.BIN %CDROOT_PATH%\BOR_NTSC.BIN

:make_pal
set PAL=1
echo Building PAL binary...
%MAKE% clean
%MAKE% PAL=1
move 1ST_READ.BIN %CDROOT_PATH%\BOR_PAL.BIN

set BORDATA=%CDROOT_PATH%\BOR.PAK
if not exist %BORDATA% goto make_data
goto finalize

:make_data
cd ..
%BORPAK% -d %SCRIPT_PATH%\data -b %BORDATA%
goto finalize

:finalize
echo.
echo Done!
goto end

:err_bootfile
echo No boot file (1ST_READ.BIN) was found!
echo Please build it with KallistiOS toolchain from the src/boot directory.
goto end

:err_bootstrap
echo No bootstrap file (IP.BIN) was found!
goto end

:end
pause
