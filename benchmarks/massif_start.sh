cd `dirname %0`

export KREATRIX_MODULES_PATH="../modules"

KREATRIX=../src/kreatrix
SCRIPT=run3.kx

i=1

for file in `cat test.list`; do
   testname=`basename $file`
   echo Running ... $testname

   valgrind &> /dev/null --tool="massif" $KREATRIX $SCRIPT $file &
   pid=$!
   wait $pid
   mv massif.out.$pid results/$testname.massif.out
done
