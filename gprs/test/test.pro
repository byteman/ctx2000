# -------------------------------------------------
# Project created by QtCreator 2010-04-29T09:54:30
# -------------------------------------------------
QT       -= gui core network

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
SOURCES += main.cpp ../../bycore/src/ByThread.cpp ../../mainctrl/comdata.cpp
DEFINES += 
INCLUDEPATH += ../include  ../../bycore/include ../../mainctrl ../../algo
linux-g++ {
    message(g++ = linux-g++)
    DEFINES+= 
    LIBS += -L../lib/x86
}
linux-arm-g++ {
    message(g++ = linux-arm-g++)
    DEFINES+= 
    LIBS += -L../lib/arm
}
QMAKE_CXXFLAGS+=-g
LIBS += -lgprs -lserial
DESTDIR += 
OBJECTS_DIR = 
