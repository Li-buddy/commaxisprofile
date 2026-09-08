#!/bin/bash
# Script to build CMake with toolchain for ControlX
# NOTE: This script works only with Jenkins
# Please note: Only use UNIX end-of-line style in this script (LF instead of CR+LF)

# set option: exit immediately if a command exits with a non-zero status.
set -e

# -------------------------------------------------------------------------------------------------
# echo/check arguments
# -------------------------------------------------------------------------------------------------
echo ARG 1=$1
echo ARG 2=$2
echo ARG 3=$3

NUM_CORES=$(nproc)
if [ -z "$NUM_CORES" ]; then
  echo "WARNING: nproc failed, use default -j4"
  NUM_CORES=4
fi

# -------------------------------------------------------------------------------------------------
# set CMake Setting below
# -------------------------------------------------------------------------------------------------
export HOME_DIR=$(realpath ./../..)
echo $HOME_DIR
# create cmake toolchain file
CMAKE_TOOLCHAIN_FILE="linux-"`echo $1 | cut -d'-' -f2,3`".cmake"
echo "CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE}"
export TOOLCHAINFILE=$DEVTOOLS/tool_cmake/addons/modulesX/Platform/${CMAKE_TOOLCHAIN_FILE}

mkdir -p $HOME_DIR/private/generated
export GENERATED_DIR=$HOME_DIR/private/generated

cd $GENERATED_DIR
mkdir -p $1
mkdir -p $1/$2

cd $GENERATED_DIR/$1/$2
cmake -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE=$2  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAINFILE $HOME_DIR
cmake -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE=$2  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAINFILE $HOME_DIR
make -j"$NUM_CORES" install 2>&1 | tee $GENERATED_DIR/buildlog_$1_$2.txt
mkdir -p $HOME_DIR/public/log
cp $GENERATED_DIR/buildlog_$1_$2.txt $HOME_DIR/public/log

echo Build of Platform: $1 and Configuration: $2 complete!

if test "$#" == 3 ; then
  mkdir -p $GENERATED_DIR/$1/$3
  cd $GENERATED_DIR/$1/$3
  cmake -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE=$3  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAINFILE $HOME_DIR
  cmake -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE=$3  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAINFILE $HOME_DIR
  make -j"$NUM_CORES" install 2>&1 | tee $GENERATED_DIR/buildlog_$1_$3.txt
  cp $GENERATED_DIR/buildlog_$1_$3.txt $HOME_DIR/public/log
  echo Build of Platform: $1 and Configuration: $3 complete!
fi

echo All done!
exit 0
