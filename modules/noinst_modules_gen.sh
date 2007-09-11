
# This script generates .kx file that changes paths 
# for loading modules without installation (make install)
	

KXFILE="noinst_modules_paths.kx"
DIR=`dirname $0`
echo "/*" > $KXFILE
echo "   This file changes paths for loading modules without " >> $KXFILE
echo "   instalation (make install)" >> $KXFILE
echo "*/" >> $KXFILE
echo >> $KXFILE
echo "Module paths do: {" >> $KXFILE
for i in * ; do if [ -d $i ]; then
	echo "	add: \"$DIR/$i\"." >> $KXFILE;
fi ; 
done

echo "}." >> $KXFILE


