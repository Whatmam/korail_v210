# Hybus kernel / Ramdisk Update shell script

print_usage()
{
	echo "Usage : ./update [partition] [image_name]"
	echo "		ex > update kernel zImage"
	echo "			update ramdisk ramdisk.gz"
	exit 0
}

if [ ! $2 ]; then
	print_usage
fi

if [ $1 != kernel ] && [ $1 != ramdisk ]; then
	print_usage
fi

if [ ! -s $2 ]; then
	echo "$2 is not exist."
	exit 0
fi

cnt=0

until [ $cnt -ge 5 ]; do
echo "Update the $1 with $2? [Y/n]"
read input 
case "$input" in
	Y)
		echo "Update $1 image..."
		case "$1" in 
			kernel)
				cat $2 > /dev/mtdblock1
			;;
			ramdisk)
				echo "# Erase ramdisk partition"
				flash_erase /dev/mtd2
				echo "# Write ramdisk image"
				cat $2 > /dev/mtdblock2
				
			;;
			*)	
				print_usage
			;;
		esac
		echo "# $1 image update done!"
		echo "# Reboot system!!"
		exit 0
	;;
	n)
		echo "$1 image update aborted"
	;;
	*)
		echo "Select [Y] or [n]"
		((cnt=cnt + 1))
	;;
esac
done
echo "Image update aborted"
