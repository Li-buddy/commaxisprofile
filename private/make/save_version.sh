#!/bin/bash
# Script to create a version file (based on a existing file)
# Expected parameter:
#   $1  - Build version
#   $2  - Branch
#   $3  - Git commit

VersionFile=../../CMakeLists.txt

MajorVers=$(grep -oP "set\(VERSION\s+\K\d+" < $VersionFile)
MinorVers=$(grep -oP "set\(VERSION\s+\d+\.\K\d+" < $VersionFile)
PatchVers=$(grep -oP "set\(VERSION\s+\d+\.\d+\.\K\d+" < $VersionFile)


TEMP=$(echo ${2//\//_})
Branch=$(echo ${TEMP//refs_heads_/})

echo "****************************************************"
echo "save_version.sh"
echo "  parameter:  Build=$1   Branch=$2   Commit=$3"
echo "  from parameter:  Branch=$Branch"
echo "  from $VersionFile    Major=$MajorVers   Minor=$MinorVers   Patch=$PatchVers"


BaseFile=../../public/include/comm/axisprofile/bundle_version.h
WorkPath=../generated

mkdir $WorkPath

sed -n -e '1,/VERSION_DEFINES_START/p' < $BaseFile > $WorkPath/vers_start
sed -n -e '/VERSION_DEFINES_END/,$p' < $BaseFile > $WorkPath/vers_end

echo "      $MajorVers,                                                //!< major version" > $WorkPath/vers_mid
echo "      $MinorVers,                                                //!< minor version" >> $WorkPath/vers_mid
echo "      $PatchVers,                                                //!< patch version" >> $WorkPath/vers_mid
echo "      $1,                                              //!< build version" >> $WorkPath/vers_mid
echo "      \"$Branch\",                                    //!< current branch" >> $WorkPath/vers_mid
if [ "$Branch" = "master" ]; then
  echo "      \"$MajorVers.$MinorVers.$PatchVers\",                          //!< complete version as string" >> $WorkPath/vers_mid
else
  echo "      \"$MajorVers.$MinorVers.$PatchVers-$Branch.$1\",                          //!< complete version as string" >> $WorkPath/vers_mid
fi
echo "      \"$3\"        //!< git commit" >> $WorkPath/vers_mid

    
cat $WorkPath/vers_start $WorkPath/vers_mid $WorkPath/vers_end > $WorkPath/version.h

cp -f $WorkPath/version.h $BaseFile

echo "****************************************************"
