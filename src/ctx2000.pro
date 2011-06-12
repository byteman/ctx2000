#-------------------------------------------------
#
# Project created by QtCreator 2011-01-14T16:20:55
#
#-------------------------------------------------

QT       -= gui core network

TARGET = ctx2000s

CONFIG   += console debug
CONFIG   -= app_bundle

TEMPLATE = app
DEFINES+=
MGIPATH=/media/linuxdata/works/ctx2000/gui/mg1.6/out
INCLUDEPATH+=/usr/local/include/yate guicore/include ime
INCLUDEPATH+=$$MGIPATH/include
SOURCES += \
    Main.cpp \
    MainMenu.cpp \
    ime/SoftKeyboardForm.cpp \
    ime/SoftKeyboard.cpp \
    ime/skb_bmp.c \
    ime/mgphone_nav_bmp.c \
    ime/mgphone.c \
    ime/mgime.c \
    ime/keyboard.c \
    ime/jt_engine.c \
    ime/hwinput.c \
    ime/hw_engine.c \
    ime/default_kb_bmp.c \
    ime/contbmp.c \
    ime/ime_pinyin.c \
    ime/mgpti.c \
    ime/mgpti_dict.c \
    ime/mgpti_user_dict.c \
    ime/mgpti_dict_ops.c \
    ime/mgpti_helper.c \
    ime/mgiSet.cpp \
    statusicon.cpp \
    dirstatus.cpp \
    taji.cpp \
    CppSQLite3.cpp \
    tajidbmgr.cpp \
    SysSet.cpp \
    worksite.cpp \
    iniFile.cpp \
    dataacquire.cpp \
    diantai.cpp \
    mainctrl.cpp \
    formworksite.cpp \
    formcalib.cpp \
    AlarmInfoManForm.cpp \
    comdata.cpp \
    tcparam.cpp \
    divparam.cpp \
    lijuConfForm.cpp \
    MineAddBox.cpp \
    lijuctrl.cpp \
    percent.cpp \
    filter.cpp \
    extdev/gpio.cpp \
    FastStatic.cpp \
    SingleTaji1.cpp \
    SinalTaji2.cpp \
    Image.cpp \
    TajiImage.cpp \
    MsgBox.cpp \
    beeper.cpp \
    guinotifyer.cpp \
    Password.cpp \
    dt_percent.cpp
LIBS+=-lPocoFoundation

DEPENDPATH=../include
TMPOBJ=../tmpobj
linux-arm-g++ {
    message(g++ = linux-arm-g++)
    LIBS+=-L./extlib/arm -L./guicore/lib/libmx27 -lttf \
             ./extlib/arm/libQtzCollideLib.a -lstdc++  -lts \
            -L$$MGIPATH/lib
    OBJECTS_DIR = $$TMPOBJ/arm
}
linux-g++ {
    message(g++ = linux-g++)
    OBJECTS_DIR = $$TMPOBJ/x86
    LIBS+=-L./extlib/x86 -L./guicore/lib/libpc ./extlib/x86/libQtzCollideLib.a
}

LIBS+=-lyate -lminigui -lmgext -lpng -ljpeg \
         -lguicore -lsqlite3 -lserial -lttf -lPocoNet  -lts
DEFINES+=
HEADERS += \
    ime/wt_input.h \
    ime/SoftKeyboardForm.h \
    ime/SoftKeyboard.h \
    ime/mgphone.h \
    ime/mgime_tab.h \
    ime/mgime.h \
    ime/keyboard.h \
    ime/jt_input.h \
    ime/ime_pinyin.h \
    ime/hwinput.h \
    ime/hw_engine.h \
    ime/hw.h \
    ime/config.h \
    statusicon.h \
    dirstatus.h \
    ctx2000.h \
    taji.h \
    collalgo.h \
    CppSQLite3.h \
    tajidbmgr.h \
    SysSetItem.h \
    SysSet.h \
    worksiteitem.h \
    worksite.h \
    dataacquire.h \
    diantai.h \
    mainctrl.h \
    formworksite.h \
    formcalib.h \
    formcalibitem.h \
    AlarmInfoManForm.h \
    AlarmInfoManFormItem.h \
    comdata.h \
    tcparam.h \
    divparam.h \
    divparamitem.h \
    lijuConfForm.h \
    lijuConfFormItem.h \
    ModeAddBox.h \
    ModeAddBoxItem.h \
    MineAddBox.h \
    MineAddBoxItem.h \
    lijuctrl.h \
    percent.h \
    filter.h \
    extdev/gpio.h \
    FastStatic.h \
    SingleTaji1Item.h \
    SingleTaji1.h \
    SinalTaji2.h \
    Image.h \
    TajiImage.h \
    MsgBox.h \
    beeper.h \
    guinotifyer.h \
    dt_percent.h
