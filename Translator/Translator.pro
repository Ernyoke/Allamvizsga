#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T21:43:06
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia widgets
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Translator
TEMPLATE = app


SOURCES += main.cpp\
        gui.cpp \
    listenner.cpp \
    speaker.cpp \
    g711.cpp \
    settings.cpp

HEADERS  += gui.h \
    listenner.h \
    speaker.h \
    g711.h \
    settings.h

FORMS    += gui.ui \
    settings.ui
