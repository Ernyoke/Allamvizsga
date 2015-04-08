#-------------------------------------------------
#
# Project created by QtCreator 2014-04-02T20:51:24
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia widgets
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Listenner
TEMPLATE = app


SOURCES += main.cpp\
        gui.cpp \
    settings.cpp \
    g711.cpp \
    listener.cpp \
    recordwav.cpp \
    recordaudio.cpp \
    datagram.cpp \
    soundchunk.cpp \
    logindialog.cpp \
    channelmodel.cpp \
    servercommunicator.cpp \
    channelinfo.cpp \
    channellistexception.cpp \
    addnewchannelfromgui.cpp \
    baseexception.cpp \
    invalididexception.cpp

HEADERS  += gui.h \
    settings.h \
    g711.h \
    listener.h \
    recordwav.h \
    recordaudio.h \
    datagram.h \
    soundchunk.h \
    logindialog.h \
    channelmodel.h \
    servercommunicator.h \
    channelinfo.h \
    channellistexception.h \
    addnewchannelfromgui.h \
    baseexception.h \
    invalididexception.h

FORMS    += gui.ui \
    settings.ui \
    logindialog.ui \
    addnewchannelfromgui.ui

CONFIG += c++11
