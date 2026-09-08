@setlocal enableextensions enabledelayedexpansion
@echo off

REM required parameters
REM   INPUT_FILE       what was the CppCheck result file
REM   OUTPUT_FILE      here the process junit file ist put

set INPUT_FILE=%1
set OUTPUT_FILE=%2

echo Running processCPPCheck.bat
echo   param 1 (input): %1
echo   param 2 (output): %2
echo   running in %cd%

if NOT exist %INPUT_FILE% (
  echo Did not find %INPUT_FILE%! Exit now!
  exit /b 1
)

if NOT exist private\log\ (
  mkdir private\log
  echo Created log folder.
) else (
  echo Log folder exists.
)

SET ERROR_COUNT=0
FOR /F "usebackq tokens=*" %%I IN (%INPUT_FILE%) do (
    SET LN=%%I
    FOR %%J IN ("!LN!") do (
        FOR /F %%K IN ('ECHO %%J ^| FIND /I /C "</error>"') DO (
            @SET /A ERROR_COUNT=!ERROR_COUNT!+%%K
        )
    )
) 2>NUL
ECHO Number of errors is !ERROR_COUNT!

FINDSTR /R /C:"<error .*" /C:"</error>" /C:"<location " %INPUT_FILE% > private\log\cppcheck_body.xml
if NOT exist private\log\cppcheck_body.xml echo No errors found > private\log\cppcheck_body.xml

set INTEXTFILE=private\log\cppcheck_body.xml
set OUTTEXTFILE=private\log\cppcheck_body.txt

del %OUTTEXTFILE% 2>NUL
for /f "tokens=1,* delims=¶" %%A in ( '"type %INTEXTFILE%"') do (
  SET string=%%A
  SET modified1=!string:^<error =ERROR !
  SET modified2=!modified1:^<location =LOCATION !
  SET modified3=!modified2:^</error^>=----!
  SET modified4=!modified3:/^>=.!
  SET modified5=!modified4:^>=.!
  echo !modified5! >> %OUTTEXTFILE%
)
if NOT exist %OUTTEXTFILE% echo No errors found >%OUTTEXTFILE%

echo ^<?xml version=^"1.0^" encoding=^"ISO-8859-1^"?^> > private/log/temp.xml
echo ^<?xml-stylesheet?^> > private/log/temp.xml
echo ^<testsuite name=^"CppCheck^" timestamp=^"2017-10-19T06:48:21^" tests=^"1^" errors=^"!ERROR_COUNT!^" time=^"7.2420^"^> >> private/log/temp.xml
echo   ^<testcase name=^"CppCheck.CppCheck^" classname=^"CppCheck.CppCheck^" time=^"0.4550^"^> >> private/log/temp.xml
IF /I "%ERROR_COUNT%" NEQ "0" (
  echo   ^<error message=^"CppCheck found %ERROR_COUNT% errors^"/^> >> private/log/temp.xml
)
echo     ^<system-out^> >> private/log/temp.xml
copy /B private\log\temp.xml + %OUTTEXTFILE% %OUTPUT_FILE% >NUL
echo     ^</system-out^> >> %OUTPUT_FILE%
echo   ^</testcase^> >> %OUTPUT_FILE%
echo ^</testsuite^> >> %OUTPUT_FILE%

del %INTEXTFILE%
del %OUTTEXTFILE%
del private\log\temp.xml
