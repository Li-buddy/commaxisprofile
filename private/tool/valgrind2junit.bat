@echo off

%WINDIR%\SysWOW64\cscript //nologo //Job:valgrind2junit valgrind2junit.wsf memcheck.xml index.html junit_memcheck.xml
