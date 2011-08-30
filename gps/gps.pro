#-------------------------------------------------
#
# Project created by QtCreator 2011-06-28T20:15:12
#
#-------------------------------------------------

QT       -= core gui network

TARGET = gps
TEMPLATE = lib
OUTPUT=./lib
DEFINES+=GPRS_DEBUG
INCLUDEPATH+=./include ../bycore/include 
DEPENDPATH=$$INCLUDEPATH
SOURCES += \  
    src/gps.cpp \ 
    ../bycore/src/bythread.cpp

HEADERS += \ 
    gps.h 
linux-arm-g++ {
    message(g++ = linux-arm-g++)
    OBJECTS_DIR = ./tmpobj/arm
    DESTDIR = $$OUTPUT/arm
}
linux-g++ {
    message(g++ = linux-g++)
    OBJECTS_DIR = ./tmpobj/x86
    DESTDIR = $$OUTPUT/x86
    DEFINES+=PC_DEBUG
}
LIBS+=-lPocoFoundation -lPocoNet
