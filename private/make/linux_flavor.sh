#!/bin/bash
# Script to get the linux flavor
# Please note: Only use UNIX end-of-line style in this script (LF instead of CR+LF)


LSB_RELEASE_PROGRAM="lsb_release"
if [ $(type -t $LSB_RELEASE_PROGRAM) == file ]
then
  # lsb_release -si --> "Debian" "Raspbian" "Ubuntu"
  LSB_DISTRIBUTOR_ID=`$LSB_RELEASE_PROGRAM -si`
  if [ ! $LSB_DISTRIBUTOR_ID ]
  then
    LSB_DISTRIBUTOR_ID="linux_dist"
    exit 0
  fi
  
  # formating distribution
  LSB_DISTRIBUTOR_ID=`awk '{ print tolower($0) }' <<< $LSB_DISTRIBUTOR_ID`
  
  # only for supported distrubutions...
  if [[ $LSB_DISTRIBUTOR_ID == "ubuntu" || $LSB_DISTRIBUTOR_ID == "raspian" || $LSB_DISTRIBUTOR_ID == "debian" ]];
  then
    # lsb_release -sr --> "10" "8.0" "20.04"
    LSB_RELEASE_ID=`$LSB_RELEASE_PROGRAM -sr`
    #echo $LSB_RELEASE_ID
    if [ $LSB_RELEASE_ID ]
    then
      # formating release number, get major number
      LSB_RELEASE_ID=`echo $LSB_RELEASE_ID | cut -d'.' -f1`
    else
      echo $LSB_DISTRIBUTOR_ID
      exit 0
    fi
    if [[ $LSB_DISTRIBUTOR_ID == "ubuntu" && $LSB_RELEASE_ID == "18" ]];
    then
      # compatible mode for Ubuntu 18.04
      echo linux
    else
      echo $LSB_DISTRIBUTOR_ID$LSB_RELEASE_ID
    fi
  else
    echo $LSB_DISTRIBUTOR_ID
  fi
else
  echo "linux_unknown"
fi
