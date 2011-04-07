#-------------------------------------------------
#
# Project created by QtCreator 2011-01-14T22:20:25
#
#-------------------------------------------------

QT       -= core gui network

TARGET = extdev
TEMPLATE = lib

DEFINES += EXTDEV_LIBRARY
INCLUDEPATH+=/usr/local/include/yate

SOURCES += \
    extctrl.cpp \
    gpio.cpp

HEADERS += \
    extctrl.h \
    gpio.h

TMPOBJ=../tmpobj
linux-arm-g++ {
    message(g++ = linux-arm-g++)
    LIBS+=-L../extlib/arm
}
linux-g++ {
    message(g++ = linux-g++)
    DEFINES+=_PC_DBG
}
LIBS+=-lyate -lPocoFoundation
DESTDIR=../plugin

