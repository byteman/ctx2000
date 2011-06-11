ROOTFS="/media/linuxdata/home/byteman/works/mx27/rootfs/rootfs-hitech/ctx2000"
rm $ROOTFS/resource -rf
cp resource $ROOTFS/ -rf
cp src/guicore/lib/libmx27/* $ROOTFS/bin/guicore/lib/libmx27 -a
cp src/extlib/arm/* $ROOTFS/bin/extlib/arm -a

cp src/bd.red  $ROOTFS/bin/ -a
arm-926ejs-linux-strip src/ctx2000s
cp src/ctx2000s $ROOTFS/bin -a
cp src/ctx2000.ini $ROOTFS/bin -a
cp src/ctx2000.sqlite3 $ROOTFS/bin -a
cp src/run.sh $ROOTFS/bin -a
cp src/tc.red $ROOTFS/bin -a
cp src/tcorder.red $ROOTFS/bin -a
