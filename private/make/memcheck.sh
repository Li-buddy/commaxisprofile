#!/bin/bash
# Script to build CMake with toolchain for ControlX
# NOTE: This script works only with Jenkins
# Please note: Only use UNIX end-of-line style in this script (LF instead of CR+LF)

# set option: exit immediately if a command exits with a non-zero status.
set -e

echo ARG 1=$1
echo ARG 2=$2

# -------------------------------------------------------------------------------------------------
# prepare Test
# -------------------------------------------------------------------------------------------------

echo --------------------- Prepare Test ------------------------------
HOME_DIR=$(realpath ./../..)
echo $HOME_DIR

mkdir -p $HOME_DIR/private/generated
GENERATED_DIR=$HOME_DIR/private/generated

cd $GENERATED_DIR
cd ./test/$1/debug/

LINUXTESTBUDDYPATH=$HOME_DIR/private/dependencies/apptestbuddy/public/src/linux/
echo LINUXTESTBUDDYPATH=$LINUXTESTBUDDYPATH

export LD_LIBRARY_PATH=$(realpath .)
echo $LD_LIBRARY_PATH

echo valgrind --leak-check=yes --track-origins=yes --num-callers=100 --xml=yes --xml-file=$GENERATED_DIR/$1/debug/memcheck.xml --suppressions=$LINUXTESTBUDDYPATH/valgrind.supp ./$2 ${@:3}
valgrind --leak-check=yes --track-origins=yes --num-callers=100 --xml=yes --xml-file=$GENERATED_DIR/$1/debug/memcheck.xml --suppressions=$LINUXTESTBUDDYPATH/valgrind.supp ./$2 ${@:3}
echo -e "\n\n"

mkdir $GENERATED_DIR/memcheck
mv $GENERATED_DIR/$1/debug/memcheck.xml $GENERATED_DIR/memcheck/memcheck.xml
cp $HOME_DIR/private/tool/valgrind2junit.* $GENERATED_DIR/memcheck

echo All done!
exit 0