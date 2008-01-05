cd `dirname %0`

export KREATRIX_MODULES_PATH="../modules"

KREATRIX=../src/kreatrix

if [ "$#" \< 1 ]; then
MAIN_RUNS=10
else
MAIN_RUNS=$1
fi

if [ "$#" \< 2 ]; then
TEST_RUNS=10
else
TEST_RUNS=$2
fi


echo MAIN_RUNS ... $MAIN_RUNS
echo TEST_RUNS ... $TEST_RUNS

LIST="test.list"

echo "Version ... `$KREATRIX -c "VM version println"`";
echo "Threads ... `$KREATRIX -c "(VM hasSupport: \\"threads\\") println"`";

echo "TEST: 1"
sh "test_all.sh" $KREATRIX $TEST_RUNS > tmp1
cat tmp1

for ((i=2;i<=MAIN_RUNS;i++)) do
	echo "TEST: $i"
	sh "test_all.sh" $KREATRIX $TEST_RUNS > tmp2
	cat tmp2
	$KREATRIX getmin.kx tmp1 tmp2 > tmp3
	mv tmp3 tmp1
done

paste $LIST tmp1 -d ':' > result
