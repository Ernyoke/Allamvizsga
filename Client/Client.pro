#-------------------------------------------------
#
# Project created by QtCreator 2014-04-01T22:25:10
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia widgets
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp\
        gui.cpp \
    managevoice.cpp \
    settings.cpp \
    g711.cpp

HEADERS  += gui.h \
    managevoice.h \
    settings.h \
    g711.h

FORMS    += gui.ui \
    settings.ui
