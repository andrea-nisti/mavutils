QT       += core network serialport
QT       += gui
QT       += widgets

TARGET = mocap2mav
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH +=  .     \
                ..


SOURCES += \
    main_mocap.cpp \
    NatNetReceiver.cpp \
    PositionDispatcher.cpp \
    Window.cpp \
    ManualControl.cpp \
    AutoControl.cpp \
    ManualThread.cpp \
    CommanderThread.cpp


HEADERS += \
        NatNetReceiver.h \
        PositionDispatcher.h \
        MavState.h \
    Window.h \
    ManualControl.h \
    global.h \
    utils.h \
    AutoControl.h \
    ManualThread.h \
    CommanderThread.h
