#
# test/test.pro  - DuySorter 无UI ARM64 最小测试程序
#

TARGET   = test_arm
TEMPLATE = app

CONFIG  += console c++14
CONFIG  -= app_bundle
QT      += core network sql widgets

ROOT = $$PWD/..

MOC_DIR      = buildmin/moc
OBJECTS_DIR  = buildmin/obj
RCC_DIR      = buildmin/rcc
DESTDIR      = release

# ---------- 头文件搜索路径 ----------
INCLUDEPATH += $$ROOT \
               $$ROOT/src \
               $$ROOT/src/common \
               $$ROOT/src/params \
               $$ROOT/src/3rdparty \
               $$ROOT/src/3rdparty/unilog/arm/include \
               $$ROOT/src/3rdparty/spdlog/arm/include
DEPENDPATH  += $$ROOT/src $$ROOT/src/common

# ---------- 平台/分辨率宏 ----------
DEFINES += LCD_WIDTH=1024 \
           LCD_HEIGHT=768 \
           AI_MODE=0 \
           PLATFORM=1 \
           DEBUG_OUTPUT=1

# ---------- 源文件 ----------
SOURCES += $$PWD/test_main.cpp \
           $$ROOT/src/globalflow.cpp \
           $$ROOT/src/myqextserialport.cpp \
           $$ROOT/src/aihelper.cpp \
           $$ROOT/src/cmdworker.cpp \
           $$ROOT/src/cmdudpmanager.cpp \
           $$ROOT/src/configmgr.cpp \
           $$ROOT/src/myicon.cpp \
           $$ROOT/src/aicommunicate.cpp \
           $$ROOT/src/params/paramscamera.cpp \
           $$ROOT/src/params/paramsengineer.cpp \
           $$ROOT/src/params/paramsglobal.cpp \
           $$ROOT/src/params/paramslang.cpp \
           $$ROOT/src/params/paramsprofile.cpp \
           $$ROOT/src/params/paramsshare.cpp \
           $$ROOT/src/common/globalconfig.cpp \
           $$ROOT/src/common/mylabel.cpp \
           $$ROOT/src/common/mypushbutton.cpp \
           $$ROOT/src/common/mymessagebox.cpp \
           $$ROOT/src/mydevmonitor.cpp \
           $$ROOT/src/common/mono_shared/monoflow.cpp \
           $$ROOT/src/common/mono_shared/camerasignal/ejecttimes.cpp \
           $$ROOT/src/common/mono_shared/camerasignal/ejecttimesthread.cpp \
           $$ROOT/src/rgb/material.cpp \
           $$ROOT/src/sqlitemgr.cpp \
           $$ROOT/src/udpworker.cpp \
           $$ROOT/src/mycurve.cpp \
           $$ROOT/src/common/mylcdnumber.cpp \
           $$ROOT/src/common/myinputpanel.cpp \
           $$ROOT/src/common/mygroupbox.cpp \
           $$ROOT/src/common/mylineedit.cpp \
           $$ROOT/src/common/myslider.cpp \
           $$ROOT/src/3rdparty/qextserialport/qextserialport.cpp \
           $$ROOT/src/3rdparty/qextserialport/qextserialport_unix.cpp \
           $$ROOT/src/3rdparty/qjson/qjson.cpp

HEADERS += $$ROOT/src/globalflow.h \
           $$ROOT/src/myqextserialport.h \
           $$ROOT/src/cmdudpmanager.h \
           $$ROOT/src/udpworker.h \
           $$ROOT/src/mycurve.h \
           $$ROOT/src/mydevmonitor.h \
           $$ROOT/src/common/mylcdnumber.h \
           $$ROOT/src/common/myinputpanel.h \
           $$ROOT/src/common/mylineedit.h \
           $$ROOT/src/common/mymessagebox.h \
           $$ROOT/src/common/mygroupbox.h \
           $$ROOT/src/common/myslider.h \
           $$ROOT/src/common/mylabel.h \
           $$ROOT/src/common/mono_shared/monoflow.h \
           $$ROOT/src/common/mono_shared/camerasignal/ejecttimes.h \
           $$ROOT/src/common/mono_shared/camerasignal/ejecttimesthread.h \
           $$ROOT/src/3rdparty/qextserialport/qextserialport.h

# ---------- 链接库 ----------
LIBS    += -L$$ROOT/src/3rdparty/unilog/arm/lib -lunilog
