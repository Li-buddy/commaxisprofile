@echo off
REM -------------------------------------------------------------------------------------------------
REM echo/check arguments
REM -------------------------------------------------------------------------------------------------
echo ARG 0=%0
echo ARG 1=%1
echo ARG 2=%2
echo ARG 3=%3

set CWD=%cd%

cd %~dp0

set FLATC_EXECUTABLE_RELATIVE=.\..\dependencies\oss.flatbuffers\public\bin\win-msvc-x64\debug
set FLATC_EXECUTABLE=
pushd %FLATC_EXECUTABLE_RELATIVE%
set FLATC_EXECUTABLE=%cd%
popd

set fbsDestPath_rel=.\..\..\public\datatypes\generated
if not exist %fbsDestPath_rel% mkdir %fbsDestPath_rel%
set fbsDestPath=
pushd %fbsDestPath_rel%
set fbsDestPath=%cd%
popd

FOR %%i IN ("%1") DO (
set filedrivepath_1=%%~dpi
set filedrive_1=%%~di
set filepath_1=%%~pi
set filename_1=%%~ni
set fileextension_1=%%~xi
)

cd %filedrivepath_1%

echo Generating flatbuffer files from:
%FLATC_EXECUTABLE%\flatc.exe -o %fbsDestPath% --schema --binary --cpp %1

cd %CDW%
echo Copy generated files to: & echo.%fbsDestPath%
echo DONE