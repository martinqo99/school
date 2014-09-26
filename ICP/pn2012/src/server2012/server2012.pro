#-------------------------------------------------
#
# Project created by QtCreator 2012-02-27T17:51:24
#
#-------------------------------------------------

QT       += core
QT       += network
QT       += gui
QT       += xml

TARGET = server2012
CONFIG   += qt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    tcpserver.cpp \
    thread.cpp \
    users.cpp \
    simulation.cpp \
    place.cpp \
    arc.cpp \
    transition.cpp

HEADERS += \
    tcpserver.h \
    thread.h \
    users.h \
    simulation.h \
    place.h \
    arc.h \
    transition.h

RESOURCES += \
    myres.qrc



























