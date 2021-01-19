#!/bin/sh

if [ $1 ]; then
if [ $1 -lt 101 ] && [ $1 -gt -1 ]; then
	echo "LEFT="0%" RIGHT="$1%"" > /root/config/soundconf
	amixer -c 0 sset Speaker,0 0% $1% on
	exit 0
fi
fi
	echo "usage : volume [value]"
	echo "		value : 0~100"
