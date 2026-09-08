#!/bin/bash
# this script is a work around for rsync generated useless sync folder
cd $(realpath $(dirname "$0")/../../..)
for i in $(ls); do
  if [ -d "$i" ]; then

    cd "$i"

    # count total number of files
    files_counts=$( ls | wc -l )

    # check whether rsync files are there
    if [ $( ls | grep "rsyncd.conf" ) ]; then rsyncd_conf=1; fi
    if [ $( ls | grep "rsyncd.log" ) ]; then rsyncd_log=1; fi
    if [ $( ls | grep "rsyncd.pid" ) ]; then rsyncd_pid=1; fi

    cd ..

    # remove folder when total number of files matches with numbers of rsync files
    if [ $files_counts -eq $((rsyncd_conf+rsyncd_log+rsyncd_pid)) ] ; then
      rm -rf $i
    fi
  fi
done;
