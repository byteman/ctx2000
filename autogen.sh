#!/bin/bash
TOPDIR=$PWD
DT1000OBJS="mgcore datacopy cppsqlite3 gui"
QMAKEARGS="-spec qws/linux-arm-g++"
TMP_DIR=tmpobj
DO_STRIP=0
DO_CLEAN=0
DO_COMPILE=1
ARCH="x86"
ARCH_LIBPATH=../lib
OPT="CONFIG=release"
#OPT="CONFIG=debug"

#set -e
#set -x


function isARMVersion()
{
    if [ $# -eq 0 ];then
        return 1;
    fi
	echo "guess: " $1 "type"
    file $1 | grep ARM 1>/dev/null
    if [ $? -eq 0 ] ;then
       return 1; 
    else
       return 0; 
    fi
}
# -eq 表达式只能用于数字比较
# == 可以测试字符串
function isNeedClean()
{
	for loop in `ls *.o 2>/dev/null` 
	do
		isARMVersion $loop
		if [ $? -eq 1 ];then
		   echo $loop "ARM Version" 
		   if [ -z $MX27 ];then #不是arm架构，清除全部
		   		return 1;
		   fi
		else
		   echo $loop "X86 Version"
		   if [ "x$MX27" == "x1" ];then #是arm架构，清除全部
		   		return 1;
		   fi
		fi	
	done
	
	return 0
}


function do_clean()
{
    del_objs=$TMP_DIR/$ARCH/*
    echo "del $del_objs" -rf
    #rm $del_objs
    
    for var in ${DT1000OBJS}
    do
       cd $var && make clean && cd $TOPDIR 
    done
}

function do_compile()
{
    
    for var in ${DT1000OBJS}
    do
       
       cd $var && qmake $QMAKEARGS && make -j 4  && cd $TOPDIR 
    done
}
function do_strip()
{
    strip1="$ARCH_LIBPATH/"
    if [ "$ARCH" == "arm" ];then
        cd $TOPDIR
        pwd=$PWD
        
        arm-926ejs-linux-strip $pwd/ctx2000/lib/lib*.so.*
        arm-926ejs-linux-strip $pwd/ctx2000/bin/ctx2000
    else
        #strip "$ARCH_LIBPATH/libmx27"
        #strip "../bin/guiApp-arm"
        echo "strip $TOPDIR/lib/libpc"
        echo "strip ../bin/guiApp-pc"
    fi
}




if [ $# == 0 ]; then
    echo "usage  autogen.sh [arm|x86],[strip|clean]"
    exit 0
fi

var1=`tr '[A-Z]' '[a-z]' <<<"$1"`

if [ "$var1" == "arm" ];then
    ARCH="arm"
    QMAKEARGS="-spec qws/linux-arm-g++ "$OPT""
else
    ARCH="x86"
    QMAKEARGS="-spec linux-g++ "$OPT""
fi

if [ ! -z $2 ]; then
    var2=`tr '[A-Z]' '[a-z]' <<<"$2"`
    if [ "$var2" == "strip" ];then
        
        DO_STRIP=1
    elif [ "$var2" == "clean" ];then
        do_clean
        exit 0
        DO_CLEAN=1
    fi
fi

do_compile 
if [ $DO_STRIP == 1 ];then
    do_strip
fi

if [ $ARCH == "x86" ];then
    cd ../bin && ./run-pc.sh
	echo "run"
fi




