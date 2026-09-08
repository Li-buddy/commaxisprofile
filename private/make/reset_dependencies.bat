echo off
pushd %~dp0
echo ---- njecting dependencies ----

rem ### Enable Command extension and check for dev tools
VERIFY OTHER 2>nul
SETLOCAL ENABLEEXTENSIONS
IF ERRORLEVEL 1 ECHO Could not find command extensions. Please contact the project maintainer!
IF DEFINED DEVTOOLS (echo DEVTOOLS are located at %DEVTOOLS%) ELSE (echo Could not find installed DEVTOOLS. Please install the DEVTOOLS to use auto generated makefiles!)
ENDLOCAL

call %DEVTOOLS%\tool_nject\prepare.bat

cd ..\..
call nject reset -manifest cdf.used.xml

popd

if %errorlevel% neq 0 exit /b %errorlevel%
