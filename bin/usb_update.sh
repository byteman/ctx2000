#!/bin/bash
src_upfile="/media/usb0/update.tar.gz"
dest_dir="/ctx2000"
dest_upfile="$destdir/update.tar.gz"
is_find_usb()
{
	cat /proc/mounts | grep "/media/usb0"
	return $?
}


is_find_usb 
if [ $? != 0 ];then
	echo "do not exit usb"
	exit 2
fi
if [ ! -e $src_upfile ];then
	echo "do not ext $src_upfile"
	exit 1
fi

if [ ! -e $dest_dir ];then
	echo "do not ext $dest_dir"
	mkdir -p $dest_dir
fi

tar zxvf $src_upfile -C / && exit 0

#cp $src_upfile $dest_upfile
#tar zxvf $src_upfile 




