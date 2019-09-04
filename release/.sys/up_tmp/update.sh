#!/bin/sh
# $1 -- reboot. $2 -- debug
# cur_dir=/tmp/upf

echo $0 $1 $2
echo "" > up_result
chmod +x md5sum
if [ -f check.md5 -a -s check.md5 ]; then
	echo Check MD5 sum
	chmod +x md5sum
	./md5sum -c check.md5
	if [ $? -ne 0 ]; then
		echo "Check MD5 sum failure!" >> up_result
		exit 1
	fi
fi

if [ $2 = 1 ]; then
	exit 0
fi

echo "Start update, please waiting..."
sys_mngr quit
sleep 2
sys_mngr wtd_close

if [ -f dpqnet_mn -a -s dpqnet_mn ]; then
  chmod +x dpqnet_mn
  echo "mv dpqnet_mn /home/boyuu"
  mv dpqnet_mn /home/boyuu
fi
if [ -f dpqnet_gui -a -s dpqnet_gui ]; then
  chmod +x dpqnet_gui
  echo "mv dpqnet_gui /home/boyuu/gui"
  mv dpqnet_gui /home/boyuu/gui
fi
if [ -f upsys.sh -a -s upsys.sh ]; then
    chmod +x upsys.sh
    ./upsys.sh
fi
if [ -d svx ]; then
    chmod +x svx/sv_rx_armlinux
    echo "mv svx/* /home/boyuu/svx"
	mv svx/* /home/boyuu/svx
fi

if [ -f logo.bmp -a -s logo.bmp ]; then
  echo "flashcp logo.bmp /dev/mtd3"
  flashcp logo.bmp /dev/mtd3 > /dev/null
fi

if [ -f version.inf -a -s version.inf ]; then
  echo "mv version.inf /home/boyuu"
  mv version.inf /home/boyuu
fi

if [ -d boyuu61850 ]; then
	cp -r boyuu61850 /home/boyuu/save/
fi
if [ -d data_sv ]; then
	cp -r data_sv /home/boyuu/save/
fi
sleep 1

if [ -f uImage -a -s uImage ]; then
  echo "nandwrite -p /dev/mtd6 uImage"
  nandwrite -p /dev/mtd6 uImage
fi