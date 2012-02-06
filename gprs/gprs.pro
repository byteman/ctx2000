#-------------------------------------------------
#
# Project created by QtCreator 2011-06-28T20:15:12
#
#-------------------------------------------------

QT       -= core gui network

TARGET = gprs
TEMPLATE = lib
OUTPUT=./lib
DEFINES+=GPRS_DEBUG
INCLUDEPATH+=./include ../bycore/include ../mainctrl ../algo ../extdev/include
DEPENDPATH=$$INCLUDEPATH
SOURCES += \  
    src/gprs.cpp \
    src/gprs_connector.cpp \
    src/gps.cpp \
    ../extdev/src/gpio.cpp \
    ../bycore/src/iniFile.cpp

HEADERS += \ 
    gprs.h \
    src/gprs_connector.h
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
    DEFINES+=PC_DEBUG
}
LIBS+=-lPocoFoundation -lPocoNet

