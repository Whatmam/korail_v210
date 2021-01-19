#!/bin/sh
 
DESTDIR=/media
AUTOPATCH=auto_patch.sh
patchdata=root.dat
patchusr=usr.dat
RAMDISK_IMAGE=ramdisk.gz

auto_patch_ramdisk()
{
	if [ -s "${DESTDIR}/$1/${AUTOPATCH}" ]; then
		"${DESTDIR}/$1/${AUTOPATCH}"
		if [ -s "${DESTDIR}/$1/${RAMDISK_IMAGE}" ]; then
			flash_erase /dev/mtd2
			cat "${DESTDIR}/$1/${RAMDISK_IMAGE}" ">" /dev/mtdblock2 
			kill_app
			dispfnd done
		fi
	fi
}

APP_NAME=kragm.exec
APP_PATH=/root/$APP_NAME
APP_PID=`ps -ef|grep $APP_NAME|grep -v grep|awk '{print $1}'`

kill_app(){
    if [ -s $APP_PID ]; then
        kill -9 $APP_PID
    fi
}

my_umount()
{
	if grep -qs "^/dev/$1 " /proc/mounts ; then
		umount "${DESTDIR}/$1";
	fi
 
	[ -d "${DESTDIR}/$1" ] && rmdir "${DESTDIR}/$1"
}
 
my_mount()
{
	mkdir -p "${DESTDIR}/$1" || exit 1
 
	if ! mount -t auto -o sync "/dev/$1" "${DESTDIR}/$1"; then
		# failed to mount, clean up mountpoint
		rmdir "${DESTDIR}/$1"
		exit 1
	fi
}
 
case "${ACTION}" in
add|"")
	my_umount ${MDEV}
	my_mount ${MDEV}
	auto_patch_ramdisk ${MDEV}
	;;
remove)
	my_umount ${MDEV}
	;;
esac

