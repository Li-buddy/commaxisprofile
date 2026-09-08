#!/bin/bash

# 1. Copy this file via MobaTerm into your /var/tmp folder
# 2. Set it's attributes to executable
# 3. Go to the console into you linux build folder (e.g. /var/tmp/comm.axisprofile)
# 4. Run '../memcheck.sh'  (you may add a test name or '-group <testgroup>' as parameters, for a single test '-test <testname>')
# 5. A file /var/tmp/memcheck.xml is generated --> valgrind results





# set option: exit immediately if a command exits with a non-zero status.
set -e

#ARG1=linux-gcc-x64
ARG1=ubuntu24-gcc-x64
ARG2=comm_axisprofile_test

echo ARG 1=$ARG1
echo ARG 2=$ARG2

# -------------------------------------------------------------------------------------------------
# prepare Test
# -------------------------------------------------------------------------------------------------

echo --------------------- Prepare Test ------------------------------
HOME_DIR=$(realpath .)
echo HOME_DIR=$HOME_DIR

DEST_DIR=$(realpath ..)
echo DEST_DIR=$DEST_DIR

LINUXTESTBUDDYPATH=$HOME_DIR/private/dependencies/apptestbuddy/public/src/linux/
echo LINUXTESTBUDDYPATH=$LINUXTESTBUDDYPATH

GENERATED_DIR=$HOME_DIR/private/generated
cd $GENERATED_DIR
cd ./test/$ARG1/debug/

export LD_LIBRARY_PATH=$(realpath .)
echo $LD_LIBRARY_PATH

valgrind --leak-check=yes --xml=yes --xml-file=$DEST_DIR/memcheck.xml --suppressions=$LINUXTESTBUDDYPATH/valgrind.supp ./$ARG2 $@
echo -e "\n\n"

echo All done!
exit 0
