#-------------------------------------------------
#
# Project created by QtCreator 2014-05-08T09:48:52
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia widgets
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Speaker
TEMPLATE = app


SOURCES += main.cpp\
        gui.cpp \
    g711.cpp \
    managevoice.cpp \
    settings.cpp \
    datagram.cpp \
    logindialog.cpp \
    newchanneldialog.cpp \
    channelinfo.cpp \
    servercommunicator.cpp \
    soundchunk.cpp

HEADERS  += gui.h \
    g711.h \
    managevoice.h \
    settings.h \
    datagram.h \
    soundchunk.h \
    logindialog.h \
    newchanneldialog.h \
    channelinfo.h \
    servercommunicator.h

FORMS    += gui.ui \
    settings.ui \
    logindialog.ui \
    newchanneldialog.ui

CONFIG += c++11
