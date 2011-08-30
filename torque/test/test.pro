# -------------------------------------------------
# Project created by QtCreator 2010-04-29T09:54:30
# -------------------------------------------------
QT       -= gui core network

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
SOURCES += main.cpp
DEFINES += 
INCLUDEPATH += ../include ../../cppsqlite3/include
linux-g++ {
    message(g++ = linux-g++)
    DEFINES+= 
    LIBS += -L../lib/x86 \
    -L../../cppsqlite3/lib/x86
}
linux-arm-g++ {
    message(g++ = linux-arm-g++)
    DEFINES+= 
    LIBS += -L../lib/arm \
            -L../../cppsqlite3/lib/arm
}
QMAKE_CXXFLAGS+=-g
LIBS += -ltorque -lcppsqlite3
DESTDIR += 
OBJECTS_DIR = 
