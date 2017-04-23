QT     += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
CONFIG += warn_on \
          qt \
          thread 
QT += network

RESOURCES += pesim.qrc

INCLUDEPATH = ../ ../inc
TARGET  = pesim
SOURCES = main.cpp
DESTDIR = ../../../bin
LIBS += -L../../../posix/lib -lpesim -ldl
