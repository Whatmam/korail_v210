#!/bin/sh

if [ -d /dev/sda1 ]; then
	echo "host device ok"
else
	echo "host device check"
	mknod /dev/sda1 b 8 1
	chmod 777 /dev/sda1
fi

if [ -d /media/sda1 ]; then
	mount /dev/sda1 /media/sda1
else
	mkdir /media/sda1
	mount /dev/sda1 /media/sda1
fi

mount -t yaffs2 -o sync /dev/mtdblock4 /media/nand1
mount -t yaffs2 -o sync /dev/mtdblock5 /media/nand2
mount -t yaffs2 -o sync /dev/mtdblock6 /media/nand3
mount -t yaffs2 -o sync /dev/mtdblock7 /media/nand4

echo 0 > /sys/module/yaffs/parameters/yaffs_auto_checkpoint




