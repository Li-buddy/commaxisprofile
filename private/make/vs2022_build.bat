echo off
echo ---- build and install x86/x64 targets for visual studio 2022 ----
 
pushd %~dp0
cd ..\..
 
REM ### Enable Command extension and check for dev tools
VERIFY OTHER 2>nul
SETLOCAL ENABLEEXTENSIONS
IF ERRORLEVEL 1 ECHO Could not find command extensions. Please contact the project maintainer!
 
REM ### Check for devtools
IF DEFINED DEVTOOLS (echo DEVTOOLS are located at %DEVTOOLS%
) ELSE (
echo Could not find installed DEVTOOLS. Please install the DEVTOOLS to use auto generated makefiles!
popd
exit /b 1
)
ENDLOCAL
 
REM ### Set build paths
set BUILDROOT=.\private\generated\msvc
set SRCROOT=.
 
call %DEVTOOLS%\tool_cmake\prepare.bat
 
REM ### Check project configuration
if NOT EXIST "CMakeLists.txt" (
@echo Cannot find "CMakeList.txt", please check your cmake project configuration
popd
exit /b 1
)
 
ECHO BUILDROOT: %BUILDROOT%
ECHO SRCDROOT: %SRCROOT%
 
if NOT EXIST "%BUILDROOT%" (
@echo Cannot find build root: "%BUILDROOT%", please check your cmake project configuration
popd
exit /b 1
)
 
REM ##############################################################################
REM ### Build and install 64-bit targets
REM ##############################################################################
set BUILDERROR=0
 
REM ### build and install release version
call :build %BUILDROOT%\x64\rel Release
if errorlevel 1 set BUILDERROR=1
 
REM ### build and install debug version
call :build %BUILDROOT%\x64\dbg Debug
if errorlevel 1 set BUILDERROR=1
 
REM ##############################################################################
REM ### Build and install 32-bit targets
REM ##############################################################################
 
REM ### build and install release version
REM call :build %BUILDROOT%\x86\rel Release
REM if errorlevel 1 set BUILDERROR=1
REM
REM REM ### build and install debug version
REM call :build %BUILDROOT%\x86\dbg Debug
REM if errorlevel 1 set BUILDERROR=1
 
popd
IF %BUILDERROR%==1 exit /b 1
exit /b
 
REM ##############################################################################
REM ### call build <buildpath> <configtype{Debug,Release}>
REM ##############################################################################
:build
echo build %1 %2
if NOT EXIST "%1" (
@echo Cannot find build path "%1", please check your cmake project configuration
popd
exit /b 1
)
 
if "%2" NEQ "Release" (
if "%2%" NEQ "Debug" (
@echo Invalid config type "%2", please check your cmake project configuration
popd
exit /b 1
)
)
set BUILDPATH=%1
set CONFIGTYPE=%2
cmake --build %BUILDPATH% --target INSTALL --config %CONFIGTYPE% -- /nologo /verbosity:minimal /maxcpucount
if errorlevel 1 exit /b 1
exit /b
