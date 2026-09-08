#!/bin/bash
# Script to build CMake with toolchain for ControlX
# NOTE: This script works only with Jenkins
# Please note: Only use UNIX end-of-line style in this script (LF instead of CR+LF)

# set option: exit immediately if a command exits with a non-zero status.
set -e


targetpath="linux-gcc-x64"
targettype="debug"
testbin="comm_axisprofile_test"


HOME_DIR=$(realpath ./../..)
echo HOME_DIR=$HOME_DIR

GENERATED_DIR=$HOME_DIR/private/generated
echo GENERATED_DIR=$GENERATED_DIR

echo try to change folder to $GENERATED_DIR/test/$targetpath/$targettype/
cd $GENERATED_DIR/test/$targetpath/$targettype/

echo running perf record -g -- ./$testbin $@
echo -----------------------------------------------------------------------------------------------------
perf record -g -- ./$testbin $@
echo -----------------------------------------------------------------------------------------------------


echo copy gprof2dot.py to current folder
cp $HOME_DIR/private/tool/gprof2dot.py .
echo try to generate profile image
perf script | c++filt | gprof2dot.py -f perf -s --skew=0.01 | dot -Tpng -Gcharset=latin1 -o profile.png

REM gprof ../../public/bin/TestLib.run ../gmon.out | gprof2dot.py -s --skew=0.01 | dot -Tpng -Gcharset=latin1 -o output.png
REM gprof ../../public/bin/TestLib.run ../gmon.out | gprof2dot.py -s --skew=0.01 | dot -Tpng -Gcharset=latin1 -o output.png
REM gprof ../../public/bin/TestLib.run ../gmon.out | gprof2dot.py -s --skew=0.0001 -z CLocalClient::doWork() | dot -Tpng -Gcharset=latin1 -o output.png

echo finished!
