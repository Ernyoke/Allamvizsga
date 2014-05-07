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
    listenner.cpp \
    settings.cpp \
    g711.cpp

HEADERS  += gui.h \
    listenner.h \
    settings.h \
    g711.h

FORMS    += gui.ui \
    settings.ui
