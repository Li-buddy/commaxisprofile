#!/bin/bash

# 1. Copy this file via MobaTerm into your /var/tmp folder
# 2. Set it's attributes to executable
# 3. Go to the console into you linux build folder (e.g. /var/tmp/comm.axisprofile)
# 4. Run '../coverage.sh'  (you may add a test name or '-group <testgroup>' as parameters)
# 5. A folder /var/tmp/html is generated, download it (via MobaTerm) to your windows
# 6. Open index.html to view the coverage




# set option: exit immediately if a command exits with a non-zero status.
set -e

# -------------------------------------------------------------------------------------------------
# check commandline parameters
# -------------------------------------------------------------------------------------------------

targetpath=private/generated/msvc-Unix_Makefiles/ubuntu24-gcc-x64/debug/private/src/test
testbin=comm_axisprofile_test
branch="Local build"
git_commit="not set"

echo "running coverage with: "
echo "  target path: $targetpath"
echo "  test binary: $testbin"
echo "  branch: $branch"
echo "  commit: $git_commit"

rm ../html -rf

# -------------------------------------------------------------------------------------------------
# prepare Test
# -------------------------------------------------------------------------------------------------

echo --------------------- Prepare Test -------------------------------
HOME_DIR=$(realpath .)
echo $HOME_DIR

# ------------------------------------------------------------------------------------------------
# get exclusion coverage list
# ------------------------------------------------------------------------------------------------

function getExclusionList {
  local lcov_exclude_file=$HOME_DIR/private/make/coverage.exclude
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

cd $GENERATED_DIR/test/ubuntu22-gcc-x64/debug/

export LD_LIBRARY_PATH=$(realpath .)
echo $LD_LIBRARY_PATH

if [ ! -f "${testbin}" ]; then
    echo "test binary '${testbin}' not found!";
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
cd $HOME_DIR
lcov -c --directory . --output-file coverage.info --test-name "AutoTest"

echo lcov --remove coverage.info $EXCLUDES -o coverage_filtered.info
lcov --remove coverage.info $EXCLUDES -o coverage_filtered.info

DATE_WITH_TIME=`date -R`
echo "AutoTest" > test.txt
echo "    Branch=$branch;" >> test.txt
echo "    Commit=$git_commit;" >> test.txt
echo "    Test finished=$DATE_WITH_TIME;" >> test.txt
gendesc test.txt --output-filename test.desc

mkdir -p ./html
genhtml --legend coverage_filtered.info -d test.desc --output-directory ../html

rm test.desc
rm test.txt

echo All done!
exit 0
