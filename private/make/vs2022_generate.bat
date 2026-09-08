echo off
echo ---- creating workspaces and makefiles for visual studio 2022 ----
 
rem ### Enable Command extension and check for dev tools
VERIFY OTHER 2>nul
SETLOCAL ENABLEEXTENSIONS
IF ERRORLEVEL 1 ECHO Could not find command extensions. Please contact the project maintainer!
IF DEFINED DEVTOOLS (echo DEVTOOLS are located at %DEVTOOLS%) ELSE (echo Could not find installed DEVTOOLS. Please install the DEVTOOLS to use auto generated makefiles!)
ENDLOCAL
 
call %DEVTOOLS%\tool_cmake\prepare.bat
 
pushd %~dp0
cd ..\..
 
if NOT EXIST "CMakeLists.txt" (
@echo Cannot find "CMakeList.txt", please check your cmake project configuration
popd
exit /b 1
)
 
REM ### Set build paths
set BUILDROOT=.\private\generated\msvc
set SRCROOT=.
set CMAKEERROR=0
 
REM ### Generate 64bit
set BUILDPATH=%BUILDROOT%\x64\rel
if not exist %BUILDPATH% mkdir %BUILDPATH%
 
cmake -S %SRCROOT% -B %BUILDPATH% -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=%DEVTOOLS%/tool_cmake/addons/modulesX/Platform/win-msvc-x64.cmake -DMSC_REQ_ADAPTER=1
IF ERRORLEVEL 1 set CMAKEERROR=1
 
cmake -S %SRCROOT% -B %BUILDPATH% -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=%DEVTOOLS%/tool_cmake/addons/modulesX/Platform/win-msvc-x64.cmake -DMSC_REQ_ADAPTER=1
IF ERRORLEVEL 1 set CMAKEERROR=1
 
set BUILDPATH=%BUILDROOT%\x64\dbg
if not exist %BUILDPATH% mkdir %BUILDPATH%
 
cmake -S %SRCROOT% -B %BUILDPATH% -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=debug -DCMAKE_TOOLCHAIN_FILE=%DEVTOOLS%/tool_cmake/addons/modulesX/Platform/win-msvc-x64.cmake -DMSC_REQ_ADAPTER=1
IF ERRORLEVEL 1 set CMAKEERROR=1
 
cmake -S %SRCROOT% -B %BUILDPATH% -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=debug -DCMAKE_TOOLCHAIN_FILE=%DEVTOOLS%/tool_cmake/addons/modulesX/Platform/win-msvc-x64.cmake -DMSC_REQ_ADAPTER=1
IF ERRORLEVEL 1 set CMAKEERROR=1
 
REM ### Generate 32bit
REM set BUILDPATH=%BUILDROOT%\x86\rel
REM if not exist %BUILDPATH% mkdir %BUILDPATH%
REM
REM cmake -S %SRCROOT% -B %BUILDPATH% -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=%DEVTOOLS%/tool_cmake/addons/modulesX/Platform/win-msvc-x86.cmake -DMSC_REQ_ADAPTER=1
REM IF ERRORLEVEL 1 set CMAKEERROR=1
REM
REM cmake -S %SRCROOT% -B %BUILDPATH% -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=%DEVTOOLS%/tool_cmake/addons/modulesX/Platform/win-msvc-x86.cmake -DMSC_REQ_ADAPTER=1
REM IF ERRORLEVEL 1 set CMAKEERROR=1
REM
REM set BUILDPATH=%BUILDROOT%\x86\dbg
REM if not exist %BUILDPATH% mkdir %BUILDPATH%
REM
REM cmake -S %SRCROOT% -B %BUILDPATH% -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=debug -DCMAKE_TOOLCHAIN_FILE=%DEVTOOLS%/tool_cmake/addons/modulesX/Platform/win-msvc-x86.cmake -DMSC_REQ_ADAPTER=1
REM IF ERRORLEVEL 1 set CMAKEERROR=1
REM
REM cmake -S %SRCROOT% -B %BUILDPATH% -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=debug -DCMAKE_TOOLCHAIN_FILE=%DEVTOOLS%/tool_cmake/addons/modulesX/Platform/win-msvc-x86.cmake -DMSC_REQ_ADAPTER=1
REM IF ERRORLEVEL 1 set CMAKEERROR=1
 
popd
IF %CMAKEERROR%==1 exit /b 1
exit /b
