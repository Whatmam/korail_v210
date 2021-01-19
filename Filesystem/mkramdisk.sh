#!/bin/sh
rm -rf tmp
rm -rf ramdisk.gz
dd if=/dev/zero of=ramdisk bs=1k count=51200
mke2fs ramdisk

mkdir tmp
mount -o loop ramdisk tmp
cp -R rootfs/* tmp/
umount tmp
gzip ramdisk
chmod -R 0777 ramdisk.gz
rm -rf tmp

