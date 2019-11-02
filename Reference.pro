TARGET = Reference
TEMPLATE = app

QT       += core gui
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
        Reference/reference.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        Reference/reference.h \
        mainwindow.h

FORMS += \
        mainwindow.ui

DISTFILES += \
    TODO.list
