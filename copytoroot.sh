#!/bin/bash
libsdir="cppsqlite3 datacopy mgcore gprs"
destdir="ctx2000"
destlib="ctx2000/lib"

if [ ! -f $destlib ]; then
    mkdir -p $destlib
fi

cp bin $destdir -a
cp extlib/arm/libQtzCollideLib.a $destlib
for loop in ${libsdir}
do
    cp -a $loop/lib/arm/* $destlib
done

cp -a resource $destdir


arm-926ejs-linux-strip $destdir/bin/ctx2000 2>/dev/null
arm-926ejs-linux-strip $destlib/* 2>/dev/null

cp -a ctx2000 /media/linuxdata/home/byteman/works/mx27/nfsroot/ctx2000
 
