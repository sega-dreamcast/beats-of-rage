@echo off

cd src

:make_ntsc
SET NTSC=1
make

:make_pal
SET PAL=1
make ../PAL/1ST_READ.BIN

pause
