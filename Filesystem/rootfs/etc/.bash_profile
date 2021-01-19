# .bash_profile		1.1 	2014/07/02	(SOLIDTEK)
#

export PATH=\
/bin:\
/sbin:\
/usr/bin:\
/usr/sbin:\
/usr/bin/X11:\
/usr/local/bin

umask 022

if [ -f ~/.bashrc ]; then
    source ~/.bashrc
fi

# User specific environment and startup programs
MACH=$HOSTNAME

r()
{
	EXECFILE=$MACH.exec
	cd /root
	if [ -f $EXECFILE ]; then
		rm -f $EXECFILE
		echo ">>> $EXECFILE removed"
	else
		echo ">>> $EXECFILE does not exist"
	fi
}

getusb()
{
	df | (
		while read dev size used avail usep mdir; do
			for i in "" 1 2 3 4; do
				if [ "$dev" == "/dev/sda$i" ]; then
					echo "$mdir"
					return
				fi
			done
		done
	)
}

usbmnt()
{
	USBDIR=/media/sda1
	for i in 1 2 3 4; do
		mount /dev/sda$i $USBDIR 2>&-
		err=$?
		if [ $err == 0 ]; then
			break
		fi
	done
	if [ $err != 0 ]; then
		mount /dev/sda $USBDIR
		err=$?
	fi
	if [ $err == 0 ]; then
		echo ">>> USB mounted on $USBDIR"
	fi
	return $err
}

usbumnt()
{
	USBDIR=`getusb`
	if [ "$USBDIR" ]; then
		umount $USBDIR
		if [ $? == 0 ]; then
			echo ">>> $USBDIR unmounted"
		fi
	fi
}

if [ `tty` == /dev/ttySAC2 ]; then
	#
	# process usb profile
	UPROFILE=$MACH.prof
	TMPDIR=/tmp
	if [ -f $TMPDIR/$UPROFILE ]; then
		UPFSIZ=`cat $TMPDIR/$UPROFILE|wc -c`
		if [ $UPFSIZ != 0 ]; then
			echo ">>> Uprofile was run already"
			dispfnd UPAR
			sleep 1
		fi
	else
		cat /dev/null > $TMPDIR/$UPROFILE
		echo ">>> Push any button to USB mount."
		dispfnd USBP 
		getpbtn 3
		if [ $? != 0 ]; then
			echo ">>> Button pushing..."
			sleep 15
			USBDIR=`getusb`
			if [ "$USBDIR" ]; then
				echo ">>> USB mounted."
				if [ -r "$USBDIR/$UPROFILE" ]; then
					echo ">>> $UPROFILE run..."
					dispfnd URUN
					cat $USBDIR/$UPROFILE > $TMPDIR/$UPROFILE  
					chmod +x $TMPDIR/$UPROFILE                
					cd $USBDIR
					$TMPDIR/$UPROFILE                         
					cd $HOME
				else
					echo ">>> $UPROFILE not found"
					dispfnd UPNF
				fi
				usbumnt
				echo ">>> USB umounted."
			else
				echo ">>> USB not inserted"
				dispfnd USBn
			fi
			sleep 1
		fi
	fi
	#
	# autoboot
	AGMEXEC=$MACH.exec
	EXECFILE=/root/$AGMEXEC
	TMPEXEC=$TMPDIR/$AGMEXEC
	FastBoot=0
	RETRY=30 # sec.
	while true; do
		echo ""
		echo -n "Load $EXECFILE."
		dispfnd Boot
		#
		# check existence
		if [ ! -r $EXECFILE ]; then
			echo ".not found"
			dispfnd NOTF
			sleep $RETRY
			continue
		fi
		#
		# check header
		if [ "`head -1 $EXECFILE|cut -d. -f3`" != "$MACH" ] ||
		   [ "`head -1 $EXECFILE|cut -d. -f4`" != "exec"  ] ||
		   [ "`head -1 $EXECFILE|cut -d. -f5|wc -c`" != 4 ]; then
		   	echo -n ".header error: "
			head -1 $EXECFILE
			dispfnd HErr
			sleep $RETRY
			continue
		fi
		echo -n "`head -1 $EXECFILE|cut -d. -f5-`.."
		#
		# check size
		HSIZ=`head -1 $EXECFILE|cut -d. -f2`
		HSIZ=`calc -u 0x$HSIZ`
		echo -n $HSIZ bytes..
		FSIZ=`tail -n+2 $EXECFILE|wc -c`
		if [ "$HSIZ" != "$FSIZ" ]; then
			echo "size error: $FSIZ"
			dispfnd SErr
			sleep $RETRY
			continue
		fi
		#
		# check crc
		HCRC=`head -1 $EXECFILE|cut -d. -f1`
		HCRC=`calc -u 0x$HCRC`
		FCRC=`tail -c+9 $EXECFILE|fcrc32`
		if [ "$HCRC" != "$FCRC" ]; then
			echo "crc error: H/$HCRC F/$FCRC"
			dispfnd CErr
			sleep $RETRY
			continue
		fi
		echo Ok
		#
		# do execution
		{ tail -n+2 $EXECFILE; head -1 $EXECFILE; } > $TMPEXEC
		chmod +x $TMPEXEC
		trap '' 2
		BgnTime=`date +%s`
		$TMPEXEC
		err=$?
		trap 2
		if [ $err == 0 ]; then
			cd /root
			dispfnd SHEL
			break # quit
		fi
		#
		#
		EndTime=`date +%s`
		RunTime=`expr $EndTime - $BgnTime`
		if [ $RunTime -le 60 ]; then
			if [ $FastBoot -lt 5 ]; then
				FastBoot=`expr $FastBoot + 1`
			else
				echo ">>> Progeam error occurred, will restart after $RETRY sec."
				dispfnd PErr
				sleep $RETRY
				FastBoot=0
			fi
		else
			FastBoot=0
		fi
	done
fi

