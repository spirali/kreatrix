
FILE1=$1
FILE2=$2

sort $FILE1 > tmp1
sort $FILE2 > tmp2

join tmp1 tmp2 -t : > tmp3

for line in `cat tmp3`; do
	name=`echo $line | cut -f 1 -d :`
	a=`echo $line | cut -f 2 -d :`
	b=`echo $line | cut -f 3 -d :`
	
	c=`expr $b - $a`
	d=`echo "scale=3; $c * 100 / $a" |bc`

	echo $name $a $b $c $d\%
done

