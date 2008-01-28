cd `dirname %0`

export KREATRIX_MODULES_PATH="../modules"

KREATRIX=../src/kreatrix
SCRIPT=run2.kx

if [ -z kreatrix.log ]; then 
	rm kreatrix.log;
fi

TESTSUPPORT=`$KREATRIX -c "(VM hasSupport: \"log\") print"`

if [ $TESTSUPPORT != "true" ] ; then
	echo VM has to be compiled with --enable-vm-log
	exit 1
fi

i=1

for file in `cat test.list`; do
   name=`basename $file`
   echo Running ... $name $i
   $KREATRIX $SCRIPT $file
   cd logparsers
   echo Processing log ... `du -hs ../kreatrix.log`
   kreatrix stat.kx ../kreatrix.log
   echo Creating graph ...
   sh create-graph.sh $file
   sh create-tex.sh $file
   mv objstat.eps ../results/$name-objstat.eps
   mv slotstat.eps ../results/$name-slotstat.eps
   mv memstat.eps ../results/$name-memstat.eps
   mv objstat.tex ../results/$name-objstat.tex
   mv memstat.tex ../results/$name-memstat.tex
   mv memhistory.eps ../results/$name-memhistory.eps
   mv out.pdf ../results/$name.pdf
   cd ..
   i=`expr $i + 1`
done
