#!/bin/sh
DEBUGER=""
PARAM=""

export KREATRIX_MODULES_PATH="../modules"

if [ "$1" == "valgrind" ]; then
	DEBUGER="valgrind"
	PARAM="-v"
fi

if [ "$1" == "verbose" ]; then
	PARAM="-v"
fi

cd `dirname $0`

TEST_COUNT=`ls test_*.kx | wc -l`
TEST_PASSED=0
for testfile in `ls test_*.kx`; do
	echo "--> TEST $testfile <--"
	$DEBUGER ../src/kreatrix $PARAM $testfile && 
		TEST_PASSED=`expr $TEST_PASSED + 1`;
	echo ""
done

echo "Version: "`../src/kreatrix | head -n 1`
echo "svnversion ... " `svnversion ..`
echo "-----> PASSED TEST: $TEST_PASSED/$TEST_COUNT <-----"
