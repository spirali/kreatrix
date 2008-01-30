
KREATRIX=$1
SCRIPT=run.kx
COUNT=$2

for file in `cat test.list`; do
	sum=0
	for ((i=1;i<=COUNT;i++)) do 
		result=`$KREATRIX $SCRIPT $file | grep "^<%%%" | cut -d' ' -f 3`
		sum=`expr $sum + $result`
	done

	avg=`expr $sum / $COUNT`
	#echo "<test filename='$file'>$avg</test>"

	echo $avg
done
