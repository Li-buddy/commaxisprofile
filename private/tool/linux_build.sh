#!/bin/bash

# This shell is used for building project in local docker.

# 1. cd /var/tmp/comm.axisprofile
# 2. Change shell mode +x, "chmod +x ./private/tool/*.sh";
# 3. Run this shell, e.g:
#       build all versions, "./private/tool/linux_build.sh"
#       build x86 debug version, "./private/tool/linux_build.sh x64 debug"
#       build arm release version, "./private/tool/linux_build.sh aarch64 release"

# set option: exit immediately if a command exits with a non-zero status.
set -e

PROJECT_NAME="comm.axisprofile"

HOME_DIR=$(realpath .)
echo HOME_DIR=$HOME_DIR

PROJECT_DIR="/var/tmp/$PROJECT_NAME"

echo "$PROJECT_DIR exist, build $PROJECT_NAME"

cd $PROJECT_DIR/private/make
chmod +x ./*.sh
LINUX_FLAVOR=`./linux_flavor.sh`

echo "Remove old build result in public of $PROJECT_DIR"
sudo rm -rf $PROJECT_DIR/public/bin
sudo rm -rf $PROJECT_DIR/public/bundle

echo "Remove private/generated folder dependencies.zip of $PROJECT_DIR"
if [ -d $PROJECT_DIR/private/generated ]; then
  find $PROJECT_DIR/private/generated -mindepth 1 ! -name "dependencies.zip" -exec rm -rf {} +
fi

if test "$#" == 0; then
echo "build all versions"
./build.sh $LINUX_FLAVOR-gcc-x64 debug release
./build.sh $LINUX_FLAVOR-gcc-aarch64 debug release

elif test "$#" == 1; then
  echo "build debug and release of $LINUX_FLAVOR-gcc-$1"
  ./build.sh $LINUX_FLAVOR-gcc-$1 debug release

else
  echo "build $2 of $LINUX_FLAVOR-gcc-$1"
  ./build.sh $LINUX_FLAVOR-gcc-$1 $2
fi

if [ $? != 0 ] ; then
  exit 1
fi
