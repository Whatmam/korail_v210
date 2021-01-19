#!/bin/sh

flash_eraseall /dev/mtd3
flash_eraseall /dev/mtd4
flash_eraseall /dev/mtd5
flash_eraseall /dev/mtd6

mount -o rw,remount -t yaffs2 /dev/mtdblock3 /media/nand1
mount -o rw,remount -t yaffs2 /dev/mtdblock4 /media/nand2
mount -o rw,remount -t yaffs2 /dev/mtdblock5 /media/nand3
mount -o rw,remount -t yaffs2 /dev/mtdblock6 /media/nand4
