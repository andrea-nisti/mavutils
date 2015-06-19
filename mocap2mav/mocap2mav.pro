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
    ManualThread.cpp \
    CommanderThread.cpp \
    MainControl.cpp \
    Commander.cpp \
    AutoThread.cpp \
    ExecThread.cpp \
    Automatic.cpp \
    Executioner.cpp


HEADERS += \
        NatNetReceiver.h \
        PositionDispatcher.h \
        MavState.h \
    Window.h \
    ManualControl.h \
    global.h \
    utils.h \
    ManualThread.h \
    CommanderThread.h \
    MainControl.h \
    Commander.h \
    AutoThread.h \
    stateParams.h \
    ExecThread.h \
    Automatic.h \
    Executioner.h
