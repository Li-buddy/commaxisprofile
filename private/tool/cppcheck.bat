call %DEVTOOLS%/prepare.bat
cppcheck.exe -j 4 --enable=warning --enable=style --enable=performance --enable=portability --inline-suppr --suppress=ignoredReturnValue -I ../../public/include -I ../dependencies/apptestbuddy/public/include ../src 2> cppcheck.log
