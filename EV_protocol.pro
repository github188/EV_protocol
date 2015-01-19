###################################################################################
#create by yanbo,QQ:1760668483
#1.目前完成了windows平台下供java调用的JNI库文件 编译方式直接QT编译 Qt4.8.6 mingw4.8.2
#2.windos平台标准C库还在进行中
###################################################################################

TARGET = EVprotocol
TEMPLATE = lib
CONFIG += console warn_on
CONFIG -= app_bundle
CONFIG -= qt





DESTDIR = build/lib
OBJECTS_DIR = build/obj


INCLUDEPATH += src/win32_api
INCLUDEPATH += src/linux_api
INCLUDEPATH += src/EV_api
INCLUDEPATH += src/general
INCLUDEPATH += src/yserialport
INCLUDEPATH += src/ytimer

INCLUDEPATH += src/java_export/include


SOURCES += \
    src/yserialport/yoc_serialbase.c \
    src/yserialport/yoc_serialport.c \
    src/java_export/com_easivend_evprotocol_EVprotocol.c \
    src/EV_api/EV_bento.c \
    src/EV_api/EV_com.c \
    src/EV_api/json.c \
    src/general/ev_config.c \
    src/ytimer/timer.c \
    src/cpp_export/EVprotocol.c

HEADERS += \
    src/yserialport/yoc_serialbase.h \
    src/yserialport/yoc_serialport.h \
    src/java_export/com_easivend_evprotocol_EVprotocol.h \
    src/EV_api/EV_bento.h \
    src/EV_api/EV_com.h \
    src/EV_api/json.h \
    src/general/ev_config.h \
    src/ytimer/timer.h \
    src/cpp_export/EVprotocol.h


#win32平台下的处理
win32{
DEF_FILE +=src/win32_api/EV_protocol.def
DEFINES += EV_STDCALL  #注意在win32平台下统一采用stdcall标准调用方式

#QMAKE_CFLAGS +=--enable-stdcall-fixup
SOURCES += \
    src/yserialport/win_yocserialport.c \

HEADERS += \
    src/yserialport/win_yocserialport.h \
}


unix{
SOURCES += \
    src/yserialport/unix_yocserialport.c \
HEADERS += \
    src/yserialport/unix_yocserialport.h \

}



