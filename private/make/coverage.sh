#!/bin/bash
# Script to build CMake with toolchain for ControlX
# NOTE: This script works only with Jenkins
# Please note: Only use UNIX end-of-line style in this script (LF instead of CR+LF)

# set option: exit immediately if a command exits with a non-zero status.
set -e

# -------------------------------------------------------------------------------------------------
# check commandline parameters
# -------------------------------------------------------------------------------------------------

function printHelp {
  echo ""
  echo "usage: "
  echo "$0 [-h] [-t targetpath] [-x testbinary ] [-b branchname] [-c commit] <...>"
  echo "$0 targetpath testbinary <...>"
  echo ""
  echo "example:"
  echo "$0 -t vs-Unix_Makefiles/linux-gcc-x64 -x testdatalayer -group 1_AutoTest -prefix linux -junitxml ../../../../log/junitlinux.xml"
}

branch="Local build"
git_commit="not set"

while getopts "ht:x:b:c:" opt; do
    case $opt in
        h)
            printHelp
            exit 0
            ;;
        t)
            targetpath="${OPTARG}"
            ;;
        x)
            testbin="${OPTARG}"
            ;;
        b)
            branch="${OPTARG}"
            ;;
        c)
            git_commit="${OPTARG}"
            ;;
    esac
done

if test $OPTIND -gt 1
then
  shift $((OPTIND-1))
else
  if [ "$#" -lt 2 ]; then
    echo "missing parameters, -h for help"
    printHelp
    exit 1
  fi
  targetpath="$1";
  testbin="$2";
  shift 2
fi

if [ -z "${targetpath}" ]; then
    echo "target path is empty or missing!"; 
    printHelp; 
    exit 1
fi
if [ -z "${testbin}" ]; then
    echo "test binary is empty or missing!"; 
    printHelp; 
    exit 1
fi

echo "running coverage with: "
echo "  target path: $targetpath"
echo "  test binary: $testbin"
echo "  branch: $branch"
echo "  commit: $git_commit"

# -------------------------------------------------------------------------------------------------
# prepare Test
# -------------------------------------------------------------------------------------------------

echo --------------------- Prepare Test -------------------------------
HOME_DIR=$(realpath ./../..)
echo $HOME_DIR

# ------------------------------------------------------------------------------------------------
# get exclusion coverage list
# ------------------------------------------------------------------------------------------------

function getExclusionList {
  local lcov_exclude_file="coverage.exclude"
  local lcov_excludes=""
  echo "info: searching for coverage exclude file '$lcov_exclude_file'"
  if [ -f $lcov_exclude_file ]; then
    while IFS= read a
    do 
    if [[ "$a" =~ [\;\#] ]] || [[ -z "${a// }" ]]
    then
      echo "skipping: $a"
      continue
    else
      lcov_excludes="$lcov_excludes $a" 
    fi
    done < $lcov_exclude_file
    echo "info: using coverage excludes: $lcov_excludes"
  else
    echo "warning: lcov exclude file '$lcov_exclude_file' missing"
  fi

  eval "$1=\$lcov_excludes"  
}

getExclusionList EXCLUDES

# ------------------------------------------------------------------------------------------------
# run coverage test
# ------------------------------------------------------------------------------------------------

echo --------------------- Running Test -------------------------------
GENERATED_DIR=$HOME_DIR/private/generated
mkdir -p $GENERATED_DIR

cd $GENERATED_DIR/test//$targetpath/debug/

export LD_LIBRARY_PATH=$(realpath .)
echo $LD_LIBRARY_PATH

if [ ! -f "${testbin}" ]; then
    echo "test binary '${testbin}' not found! "; 
    printHelp; 
    exit 1
fi

echo launching ./$testbin $@
./$testbin $@
echo -e "\n\n"

# ------------------------------------------------------------------------------------------------
# convert coverage files to html
# ------------------------------------------------------------------------------------------------

echo --------------------- Convert Files ------------------------------
cd $GENERATED_DIR/$targetpath/debug/
lcov -c --directory . --output-file coverage.info --test-name "1_AutoTest" --ignore-errors negative

echo lcov --remove coverage.info $EXCLUDES -o coverage_filtered.info --ignore-errors unused
lcov --remove coverage.info $EXCLUDES -o coverage_filtered.info --ignore-errors unused

# create test description
DATE_WITH_TIME=`date -R`
echo "1_AutoTest" > test.txt
echo "    Branch=$branch; " >> test.txt
echo "    Commit=$git_commit; " >> test.txt
echo "    Test finished=$DATE_WITH_TIME; " >> test.txt
gendesc test.txt --output-filename test.desc

mkdir -p ./html
genhtml --legend coverage_filtered.info -d test.desc --output-directory ./html

rm test.desc
rm test.txt

echo All done!
exit 0
