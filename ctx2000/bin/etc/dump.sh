#!/bin/bash

#记得导出的路径后面要加/ ，否则不成功，以后需要修复
postfix=`date +%Y%m%d%H%M%S`".csv"
alm="alarm"
rec="record"
serial="ctx2000"
usbpath="/meida/usb0/"
retcode1="0"
retcode2="0"
#echo "dump data to csv"
#echo $#
if [ $# -gt 0 ];then
	if [ ! -z $1 ];then
		serial=$1
		echo "serial="$1
	fi
	if [ $# -gt 1 ];then
	    usbpath=$2
	fi
fi
#flag=${usbpath:(-1):1} fixme
usbpath=$usbpath/ctx2000
alarm_output=$usbpath$alm"_"$serial"_"$postfix
record_output=$usbpath$rec"_"$serial"_"$postfix

if [ ! -e $usbpath ] && [ ! -d $usbpath ]; then
    echo "incorrect "$usbpath
    exit 2
fi
echo "export to $usbpath"
sqlite3 -csv -header ./etc/ctx2000.sqlite3 "select tjindex as 编号,serial as 塔机序列号, dt as 时间, dist as 幅度, weight as 起吊重量, fall as 倍率, angle as 回转角度,speed as 风速  from tblrecord" > $record_output
retcode1=$?
sqlite3 -csv -header ./etc/ctx2000.sqlite3 "select tjindex as 编号,serial as 塔机序列号, dt as 时间, dist as 幅度, angle as 回转角度 from tbalarm" > $alarm_output
retcode2=$?

if [ $retcode1 -ne 0 ] || [ $retcode2 -ne 0 ];then
    echo "dump data failed"
    exit 1
fi
exit 0
