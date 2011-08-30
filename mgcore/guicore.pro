#-------------------------------------------------
#
# Project created by QtCreator 2010-05-13T17:37:37
#
#-------------------------------------------------

QT -= core \
    gui \
    network
TARGET = mgcore
TEMPLATE = lib

DEFINES += GUICORE_LIBRARY

SOURCES += \
    src/dt_button.cpp \
    src/dt_cachebmp.cpp \
    src/dt_combox.cpp \
    src/dt_commctrl.cpp \
    src/dt_ctrl.cpp \
    src/dt_edit.cpp \
    src/dt_icon.cpp \
    src/dt_icongroup.cpp \
    src/dt_listbox.cpp \
    src/dt_listview.cpp \
    src/dt_skinbutton.cpp \
    src/dt_skinchkbutton.cpp \
    src/dt_skinctrl.cpp \
    src/dt_skinlable.cpp \
    src/dt_static.cpp \
    src/SkinForm.cpp \
    src/dt_ipaddr.cpp \
    src/GuiEng.cpp \
    src/dt_font.cpp \
    src/dt_treeview.cpp

INCLUDEPATH += ../mgi/mgiinc ./include
DEPENDPATH=$$INCLUDEPATH
OUTPUT=./lib
linux-arm-g++ {
    message(g++ = linux-arm-g++)
    OBJECTS_DIR = ./tmpobj/arm
    DESTDIR = $$OUTPUT/arm
}
linux-g++{
    message(g++ = linux-g++)
    OBJECTS_DIR = ./tmpobj/x86
    DESTDIR = $$OUTPUT/x86
}

HEADERS +=
