#-------------------------------------------------
#
# Project created by QtCreator 2012-10-19T22:51:51
#
#-------------------------------------------------

QT       += core gui

TARGET = MobileJoystickManager
TEMPLATE = app

WDK8PATH = "C:\Program Files (x86)\Windows Kits\8.0"

INCLUDEPATH += src
INCLUDEPATH += $${WDK8PATH}\Include\shared
INCLUDEPATH += $${WDK8PATH}\Include\km
INCLUDEPATH += ..\MobileJoystickDriver\MobileJoystickDriver

LIBS += Setupapi.lib
LIBS += $${WDK8PATH}\Lib\win8\um\x86\hid.lib

SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/testwindow.cpp \
    src/bluetoothwindow.cpp \
    src/bth/initwinsock.cpp \
    src/bth/bluetoothdiscoverer.cpp \
    src/bth/bluetoothio.cpp \
    src/bth/remotedevice.cpp \
    src/drv/drivermanager.cpp \
    src/drv/driver.cpp

HEADERS  += \
    src/mainwindow.h \
    src/testwindow.h \
    src/bluetoothwindow.h \
    src/bth/initwinsock.h \
    src/bth/bluetoothconfig.h \
    src/drv/driver.h \
    src/msg/messages.h \
    src/utl/logger.h \
    src/bth/bluetoothdiscoverer.h \
    src/bth/bluetoothio.h \
    src/bth/remotedevice.h \
    src/drv/drivermanager.h

FORMS    += \ 
    src/testwindow.ui \
    src/mainwindow.ui \
    src/bluetoothwindow.ui
