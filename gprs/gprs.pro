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
INCLUDEPATH+=./include ../bycore/include ../mainctrl ../algo
DEPENDPATH=$$INCLUDEPATH
SOURCES += \  
    src/gprs.cpp \
    src/gprs_connector.cpp \
    src/gps.cpp

HEADERS += \ 
    gprs.h \
    src/gprs_connector.h
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
