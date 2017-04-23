TEMPLATE = app
CONFIG += warn_on \
          qt \
          thread 
QT += network

RESOURCES += ./src/pesim.qrc

INCLUDEPATH = src inc
TARGET  = pesim
SOURCES = main.cpp
DESTDIR = ./bin
LIBS += -L./bin -lpesim -ldl
