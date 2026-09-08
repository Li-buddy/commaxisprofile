#!/bin/bash
echo off

# Any subsequent(*) commands which fail will cause the shell script to exit immediately
set -e

echo ---- check java ----
JAVA_VER=$(java -version 2>&1 >/dev/null | egrep "\S+\s+version" | awk '{print $3}' | tr -d '"')
if [ -z $JAVA_VER ]; then 
  echo "JAVA is not installed: Please install:"
  echo "sudo apt install default-jre"
  exit
else
  echo "JAVA found: $JAVA_VER"
fi

echo ---- check devtools ----
if [ -z $DEVTOOLS ]; then 
    DEVTOOLS="$HOME/git/devtools"
    echo "DEVTOOLS undefined - set to default: $DEVTOOLS"
fi

if [ -d "$DEVTOOLS" ]; then
  echo "DEVTOOLS found: ${DEVTOOLS}"
else
  echo "DEVTOOLS not found: ${DEVTOOLS}. Please clone from stash to directory ${HOME}/git:"
  echo "git clone ssh://git@lo-stash.de.bosch.com:7999/msc/devtools.git $HOME/git/devtools"
  exit
fi

echo ---- check mount directories ----
MNT_PRIMARY="/mnt/xi02fs01.xi-dept-rd/ENG6/04_BuildResults" # used for developers in Xian
MNT_SECONDARY="/mnt/lo-edepartment.mscex/BuildResults"      # used for developers in German
check_mount() {
  local mount_point="$1"
  if [ -d "$mount_point" ]; then
    if [ "$(ls -A "$mount_point")" ]; then
      echo "Found non-empty mount: $mount_point"
    else
      echo "$mount_point is empty."
      return 1
    fi
  else
    echo "$mount_point not found."
    return 1
  fi
}
# one of the mount points must be available
if check_mount "$MNT_PRIMARY"; then
  echo "$MNT_PRIMARY is available."
elif check_mount "$MNT_SECONDARY"; then
  echo "$MNT_SECONDARY is available."
else
  echo ""
  echo "Neither $MNT_PRIMARY nor $MNT_SECONDARY is mounted properly."
  echo "Please mount at least one of them."
  exit 1
fi

echo ---- get cdf.xml from root directory----
WORKING_DIR=$(realpath $(dirname "$0"))
CDF_DIR="$WORKING_DIR/../../cdf.xml"
CDF_DIR=$(realpath "$CDF_DIR")

echo ---- set nject command ----
NJECT_COMMAND=$1
if [ -z $NJECT_COMMAND ]; then
  NJECT_COMMAND="update"
  echo "NJECT_COMMAND undefined - set to default: $NJECT_COMMAND"
fi

echo ---- call nject ----
java -jar $DEVTOOLS/tool_nject/nject/nject.jar $NJECT_COMMAND -cdf $CDF_DIR
