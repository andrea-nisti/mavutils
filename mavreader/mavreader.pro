#-------------------------------------------------
#
# Project created by QtCreator 2014-09-12T11:19:25
#
#-------------------------------------------------

QT       += core serialport

QT       -= gui

TARGET = mavreader
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH = ../

SOURCES += \
    MavLinkReader.cpp \
    main_Reader.cpp \


HEADERS += \
        MavLinkReader.h
