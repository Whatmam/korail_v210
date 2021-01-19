#! /bin/bash

make distclean
make korail_v210_ramdisk_main_defconfig
make clean
make -j8
mkdir -p ./output
cp arch/arm/boot/zImage ./output/.