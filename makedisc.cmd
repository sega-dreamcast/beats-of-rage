@echo off
title Beats of Rage Rumble Edition for Sega Dreamcast: Make Disc

set OUTPUT_FILE=bor.cdi
set DATA_FILE=data.iso

set SCRIPT_PATH=%~dp0
set SCRIPT_PATH=%SCRIPT_PATH:~0,-1%
set MKISOFS=%SCRIPT_PATH%\sdk\bin\mkisofs.exe
set CDI4DC=%SCRIPT_PATH%\sdk\bin\cdi4dc.exe

if exist %OUTPUT_FILE% del %OUTPUT_FILE%
if exist %DATA_FILE% del %DATA_FILE%

%MKISOFS% -V BOR_RUMBLE_EDITION -G cd_root\IP.BIN -sysid "SEGA SEGAKATANA" -publisher "SENILE TEAM" -preparer "CDI4DC 0.3b" -appid "BEATS OF RAGE RUMBLE EDITION" -duplicates-once -sort sortfile.str -hide DISC_ID.TXT -l -o %DATA_FILE% cd_root
%CDI4DC% %DATA_FILE% %OUTPUT_FILE% -d
if exist %DATA_FILE% del %DATA_FILE%

echo Done!
pause
