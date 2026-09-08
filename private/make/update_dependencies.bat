echo off
pushd %~dp0
echo ---- njecting dependencies ----

:init 
  set "NOZIP="
  set "FILTER="

:parse
  if "%~1"==""                goto :run
  if /i "%~1"=="-nozip"       set "NOZIP=yes"     & shift & goto :parse
  if /i "%~1"=="-filter"      set "FILTER=%FILTER% %~1 %~2"    & shift & shift & goto :parse

:run

rem ### Enable Command extension and check for dev tools
VERIFY OTHER 2>nul
SETLOCAL ENABLEEXTENSIONS
IF ERRORLEVEL 1 ECHO Could not find command extensions. Please contact the project maintainer!
IF DEFINED DEVTOOLS (echo DEVTOOLS are located at %DEVTOOLS%) ELSE (echo Could not find installed DEVTOOLS. Please install the DEVTOOLS to use auto generated makefiles!)
ENDLOCAL

call %DEVTOOLS%\tool_nject\prepare.bat

cd ../..
call nject update -manifest cdf.used.xml %FILTER%

if not errorlevel 1 goto pack
color C0
echo.
echo *********************************
echo *********************************
echo *** There are errors at nject ***
echo *********************************
echo *********************************
echo.
goto end

:pack
if not defined NOZIP (
  rem zip dependencies (mode fastest)
  rem 7z u -mx=1 ./private/generated/dependencies.zip ./private/dependencies/
)

:end
popd

if %errorlevel% neq 0 exit /b %errorlevel%
