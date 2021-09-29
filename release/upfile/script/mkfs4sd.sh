#!/bin/sh
umount /media/mmcblk0p1
mkfs.ext3 /dev/mmcblk0p1

echo Reinsert the SD card...
sleep 1

fstype=?
delay=60
while [ $delay -gt 0 ]; do
  sleep 1
  fstype=`df -T /media/mmcblk0p1|awk 'NR==2 {print $2}'`
  #echo $fstype
  if [ $fstype = ext3 ]; then
    break
  fi
  delay=`expr $delay - 1`
done

if [ $delay -gt 0 ]; then
  echo Formatted successfully
else
  echo Formatting failed
fi

