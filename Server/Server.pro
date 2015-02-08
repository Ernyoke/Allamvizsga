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
    acceptdata.cpp \
    clientinfo.cpp \
    datagram.cpp \
    manageclients.cpp \
    tablemodel.cpp

HEADERS  += gui.h \
    acceptdata.h \
    clientinfo.h \
    datagram.h \
    manageclients.h \
    tablemodel.h

FORMS    += gui.ui \
    manageclients.ui

