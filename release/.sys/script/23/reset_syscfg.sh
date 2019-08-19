#!/bin/sh
#cur_dir=/tmp/upf
echo "" > up_result
chmod +x md5sum
if [ -f check.md5 -a -s check.md5 ]; then
	echo Check MD5 sum
	chmod +x md5sum
	./md5sum -c check.md5
	if [ $? != 0 ]; then
		echo "Check MD5 sum failure!" >> up_result
		exit 1
	fi
fi

echo "Start update, please waiting..."
chmod +x cold_reset
./cold_reset wtd_close
./cold_reset quit
sleep 5
killall pqm3mn

cp ./pqm3.sys.conf /boyuu/save

#echo cold_reset 4
./cold_reset 4
