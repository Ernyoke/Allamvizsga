#-------------------------------------------------
#
# Project created by QtCreator 2014-04-30T20:18:20
#
#-------------------------------------------------

QT       += core gui
QT  += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app


SOURCES += main.cpp\
        gui.cpp \
    acceptdata.cpp

HEADERS  += gui.h \
    acceptdata.h

FORMS    += gui.ui
