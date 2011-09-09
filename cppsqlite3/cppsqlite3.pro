#-------------------------------------------------
#
# Project created by QtCreator 2011-06-28T20:15:12
#
#-------------------------------------------------

QT       -= core gui network

TARGET = cppsqlite3
TEMPLATE = lib
OUTPUT=./lib
DEFINES+=CPPSQL3_DEBUG
INCLUDEPATH+=./include
DEPENDPATH=$$INCLUDEPATH
SOURCES += src/CppSQLite3.cpp
HEADERS += include/CppSQLite3.h

linux-arm-g++ {
    message(g++ = linux-arm-g++)
    OBJECTS_DIR = ./tmpobj/arm
    DESTDIR = $$OUTPUT/arm
}
linux-arm-v5te-g++ {
    message(g++ = linux-arm-v5te-g++)
    BASE=/media/linuxdata/home/byteman/works/mx27/buildroot/test/root/dt1000/platform-i.mx/sysroot-target
    INCLUDEPATH+=$$BASE/usr/include
    LIBS+=-L$$BASE/usr/lib
    OBJECTS_DIR = ./tmpobj/armv5te
    DESTDIR = $$OUTPUT/armv5te
}
linux-g++ {
    message(g++ = linux-g++)
    OBJECTS_DIR = ./tmpobj/x86
    DESTDIR = $$OUTPUT/x86
    DEFINES+=PC_DGB
}
LIBS+=-lPocoFoundation -lsqlite3
