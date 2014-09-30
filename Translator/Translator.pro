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
    speaker.cpp \
    g711.cpp \
    settings.cpp \
    listener.cpp \
    recordaudio.cpp \
    recordwav.cpp \
    worker.cpp

HEADERS  += gui.h \
    speaker.h \
    g711.h \
    settings.h \
    listener.h \
    recordaudio.h \
    recordwav.h \
    worker.h

FORMS    += gui.ui \
    settings.ui
