#!/bin/bash

#�ǵõ�����·������Ҫ��/ �����򲻳ɹ����Ժ���Ҫ�޸�
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
sqlite3 -csv -header ./etc/ctx2000.sqlite3 "select tjindex as ���,serial as �������к�, dt as ʱ��, dist as ����, weight as �������, fall as ����, angle as ��ת�Ƕ�,speed as ����  from tblrecord" > $record_output
retcode1=$?
sqlite3 -csv -header ./etc/ctx2000.sqlite3 "select tjindex as ���,serial as �������к�, dt as ʱ��, dist as ����, angle as ��ת�Ƕ� from tbalarm" > $alarm_output
retcode2=$?

if [ $retcode1 -ne 0 ] || [ $retcode2 -ne 0 ];then
    echo "dump data failed"
    exit 1
fi
exit 0
