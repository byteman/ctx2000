# -------------------------------------------------
# Project created by QtCreator 2010-04-29T09:54:30
# -------------------------------------------------
QT       -= gui core network

TARGET = ctx2000
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
SOURCES += \
    torqueForm.cpp \
    torqueInfoBox.cpp \
    Startup.cpp \
    SingleFlatForm.cpp \
    SingleDynForm.cpp \
    ../mainctrl/mainctrl.cpp \
    ../extdev/src/jdqadmin.cpp \
    ../extdev/src/gpio.cpp \
    ../extdev/src/beeper.cpp \
    ../mainctrl/comdata.cpp \
    ../tcdb/src/tajidbmgr.cpp \
    ../bycore/src/iniFile.cpp \
    ../bycore/src/bythread.cpp \
    ../dataacquire/src/filter.cpp \
    ../dataacquire/src/dataacquire.cpp \
    ../commu/src/diantai.cpp \
    guinotifyer.cpp \
    FastStatic.cpp \
    dt_percent.cpp \
    percent.cpp \
    ime/SoftKeyboardForm.cpp \
    ime/SoftKeyboard.cpp \
    ime/skb_bmp.c \
    ime/mgpti_user_dict.c \
    ime/mgpti_helper.c \
    ime/mgpti_dict_ops.c \
    ime/mgpti_dict.c \
    ime/mgpti.c \
    ime/mgphone_nav_bmp.c \
    ime/mgphone.c \
    ime/mgiSet.cpp \
    ime/mgime.c \
    ime/keyboard.c \
    ime/jt_engine.c \
    ime/ImeWnd.cpp \
    ime/ime_pinyin.c \
    ime/hwinput.c \
    ime/hw_engine.c \
    ime/default_kb_bmp.c \
    ime/contbmp.c \
    MainMenu.cpp \
    InitForm.cpp \
    worksite.cpp \
    Timer.cpp \
    taji.cpp \
    SysSet.cpp \
    SysLogForm.cpp \
    SysCfgForm.cpp \
    statusicon.cpp \
    Password.cpp \
    MsgBox.cpp \
    IPset.cpp \
    formworksite.cpp \
    formcalib.cpp \
    divparam.cpp \
    dirstatus.cpp \
    AlarmInfoManForm.cpp \
    tcparam.cpp \
    ime/mgphonetest.cpp \
    UISetForm.cpp \
    CaliBox.cpp \
    ../torque/src/torque_mgr.cpp \
    ../torque/src/torque_db.cpp
DEFINES += 
INCLUDEPATH += ../include ../mgcore/include \
               ../torque/include ../cppsqlite3/include \
               ../datacopy/include ../bycore/include\
               ../extdev/include ../mainctrl \
               ../bycore/include ../torque/include \
               ../tcdb/include ../algo ./ime\
               ../dataacquire/include ../commu/include \
               ../gprs/include
DEPENDPATH +=$$INCLUDEPATH
DESTDIR=../bin

LINKLIBS=mgcore cppsqlite3 datacopy gprs bycore
linux-g++ {
    message(g++ = linux-g++)
    DEFINES+= 
    OBJECTS_DIR = ../tmpobj/x86
    for(lib,LINKLIBS):LIBS+=-L../$$lib/x86
    LIBS += ../extlib/x86/libQtzCollideLib.a
}
linux-arm-g++ {
    message(g++ = linux-arm-g++)
    DEFINES+= 
    OBJECTS_DIR = ../tmpobj/arm
    for(lib,LINKLIBS):LIBS+=-L../$$lib/lib/arm
    LIBS += -lttf ../extlib/arm/libQtzCollideLib.a
}
linux-arm-v5te-g++ {
    message(g++ = linux-arm-v5te-g++)
    SOURCES+=../algo/QtzParam.cpp
    BASE=/media/linuxdata/home/byteman/works/mx27/buildroot/test/root/dt1000/platform-i.mx/sysroot-target
    INCLUDEPATH+=$$BASE/usr/include $$BASE/usr/local/include
    LIBS+=-L$$BASE/usr/lib -L$$BASE/usr/local/lib
    LIBS+= -lm -lpcre -lz #../extlib/arm/libQtzCollideLib.a
    for(lib,LINKLIBS):LIBS+=-L../$$lib/lib/armv5te
    OBJECTS_DIR = ./tmpobj/armv5te
    TARGET = ctx2000-armv5te
    message($$LIBS)
}
#QMAKE_CXXFLAGS+=-g
LIBS += -lmgcore -lminigui -lmgext -ljpeg \
        -lpng -lPocoFoundation -lts -lsqlite3\
        -lcppsqlite3 -lusbmount -lserial -lgprs -lPocoUtil 

HEADERS += \
    torqueForm.h \
    BmpRes.h \
    torqueInfoBoxItem.h \
    torqueInfoBox.h \
    SingleFlatFormItem.h \
    SingleFlatForm.h \
    SingleDynForm.h \
    ../mainctrl/mainctrl.h \
    ../mainctrl/comdata.h \
    ime/wt_input.h \
    ime/SoftKeyboardForm.h \
    ime/SoftKeyboard.h \
    ime/mgpti_user_dict.h \
    ime/mgpti_helper.h \
    ime/mgpti_dict_ops.h \
    ime/mgpti_dict.h \
    ime/mgpti.h \
    ime/mgphone.h \
    ime/mgiSet.h \
    ime/mgime_tab.h \
    ime/mgime.h \
    ime/keyboard.h \
    ime/jt_input.h \
    ime/ImeWnd.h \
    ime/ime_tab_pinyin.h \
    ime/ime_pinyin.h \
    ime/hwinput.h \
    ime/hw_engine.h \
    ime/hw.h \
    ime/config.h \
    UISetForm.h \
    CaliBox.h \
    ../bycore/include/LoggerMgr.h


