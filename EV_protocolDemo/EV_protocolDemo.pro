#-------------------------------------------------
#
# Project created by QtCreator 2015-01-15T10:10:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EV_protocolDemo
TEMPLATE = app



DESTDIR = ../build/lib
OBJECTS_DIR = ../build/obj
MOC_DIR = ../build/moc
UI_DIR = ../build/ui


INCLUDEPATH += ../src/cpp_export

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
        ../src/cpp_export/EVprotocol.h

FORMS    += mainwindow.ui
