#-------------------------------------------------
#
# Project created by QtCreator 2012-02-22T17:58:32
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += xml
QT       += webkit

TARGET = pn2012
CONFIG += qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    about.cpp \
    tablemodel.cpp \
    searchdialog.cpp \
    arc.cpp \
    place.cpp \
    transition.cpp \
    customgraphicsscene.cpp \
    transitionproperties.cpp \
    placeproperties.cpp \
    arcproperties.cpp \
    visualoptions.cpp \
    setserver.cpp \
    fileoptions.cpp \
    setuser.cpp

HEADERS  += mainwindow.h \
    about.h \
    tablemodel.h \
    searchdialog.h \
    arc.h \
    place.h \
    transition.h \
    customgraphicsscene.h \
    transitionproperties.h \
    placeproperties.h \
    arcproperties.h \
    visualoptions.h \
    setserver.h \
    fileoptions.h \
    setuser.h

FORMS    += mainwindow.ui \
    about.ui \
    searchdialog.ui \
    transitionproperties.ui \
    placeproperties.ui \
    arcproperties.ui \
    visualoptions.ui \
    setserver.ui \
    fileoptions.ui \
    setuser.ui

RESOURCES += \
    myres.qrc





















