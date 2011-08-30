#-------------------------------------------------
#
# Project created by QtCreator 2011-06-28T20:15:12
#
#-------------------------------------------------

QT       -= core gui network

TARGET = torque
TEMPLATE = lib
OUTPUT=./lib
DEFINES+=TQ_DEBUG
INCLUDEPATH+=./include ../cppsqlite3/include ../bycore/include

DEPENDPATH=$$INCLUDEPATH
SOURCES += \
    src/torque_db.cpp \
    src/torque_mgr.cpp
HEADERS += include/torque_db.h

DEFINES+=TC_DEBUG
linux-arm-g++ {
    message(g++ = linux-arm-g++)
    OBJECTS_DIR = ./tmpobj/arm
    DESTDIR = $$OUTPUT/arm
    LIBS+= -L../cppsqlite3/lib/arm
    
}
linux-g++ {
    message(g++ = linux-g++)
    OBJECTS_DIR = ./tmpobj/x86
    DESTDIR = $$OUTPUT/x86
    DEFINES+=PC_DGB
    LIBS+= -L../cppsqlite3/lib/x86
}
LIBS+=-lPocoFoundation -lcppsqlite3
