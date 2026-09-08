#!/bin/bash
echo ----------------------------------------
echo -START----------------------------------
echo ----------------------------------------
echo argument 0: $0
echo argument 1: $1
echo argument 2: $2
echo argument 3: $3
echo argument 4: $4
cd $(realpath $(dirname "$0")/../..)
echo ----------------------------------------
echo -tar------------------------------------
echo ----------------------------------------
#tar -czvf controlx.tar.gz public/ private/dependencies/control/public/bin/ private/dependencies/bundle/
tar -cvf controlx.tar.gz public/ private/dependencies/automationframework/public/bin/ private/dependencies/bundle/
echo ----------------------------------------
echo -mkdir----------------------------------
echo ----------------------------------------
ssh $1@$2 "rm -rf $3"
ssh $1@$2 "mkdir $3"
echo ----------------------------------------
echo -scp------------------------------------
echo ----------------------------------------
scp ./controlx.tar.gz $1@$2:$3/
echo ----------------------------------------
echo -untar----------------------------------
echo ----------------------------------------
#ssh $1@$2 "tar -xzvf $3/controlx.tar.gz -C $3"
ssh $1@$2 "tar -xvf $3/controlx.tar.gz -C $3"
ssh $1@$2 "chmod -R 0777 $3/private/dependencies/automationframework/public/bin/*"
echo ----------------------------------------
echo -END------------------------------------
echo ----------------------------------------

