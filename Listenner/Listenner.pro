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
    listener.cpp

HEADERS  += gui.h \
    settings.h \
    g711.h \
    listener.h

FORMS    += gui.ui \
    settings.ui
