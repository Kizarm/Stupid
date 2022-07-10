QT += core gui
QT += widgets

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
