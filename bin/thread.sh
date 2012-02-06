#! /bin/sh
APP=$1
cd /proc/$(pidof $APP)/task
for loop in `ls /proc/$(pidof $APP)/task/`
do
	out="";
	name="";
	out=$(cat $loop/status | grep "SleepAVG:")
	name=$(cat $loop/status | grep "Name:")
	echo "$name $out"
	
done

