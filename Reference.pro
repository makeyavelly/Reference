TARGET = Reference
TEMPLATE = app

QT       += core gui sql
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
        Reference/reference.cpp \
        main.cpp \
        mainwindow.cpp \
        sql/lastsql.cpp

HEADERS += \
        Reference/reference.h \
        mainwindow.h \
        sql/lastsql.h

FORMS += \
        mainwindow.ui

DISTFILES += \
    TODO.list
