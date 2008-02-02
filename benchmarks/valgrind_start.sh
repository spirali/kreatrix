cd `dirname %0`

export KREATRIX_MODULES_PATH="../modules"

KREATRIX=../src/kreatrix
SCRIPT=run3.kx

i=1

TESTSUPPORT=`$KREATRIX -c "(VM hasSupport: \"log\") print"`

if [ $TESTSUPPORT != "false" ] ; then
	echo VM must not be compiled with --enable-vm-log
	exit 1
fi


for file in `cat test.list`; do
   testname=`basename $file`
   echo Running ... $testname

   #test line# valgrind &> /dev/null --tool="callgrind" $KREATRIX -c "123 println" & # $SCRIPT $file &
   valgrind &> /dev/null --tool="callgrind" --dump-instr=yes --trace-jump=yes $KREATRIX $SCRIPT $file &
   pid=$!
   wait $pid
   mv callgrind.out.$pid results/$testname.callgrind.out
done
