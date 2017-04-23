TARGET = pesim
TEMPLATE = lib
#TEMPLATE = app
CONFIG += warn_on \
          thread \
          qt staticlib
QT += network

INCLUDEPATH += ../inc/

SOURCES  = 
SOURCES += mainwindow.cpp
SOURCES += DigitalIO.cpp
SOURCES += AnalogIO.cpp
SOURCES += Loop.cpp
#SOURCES += debug.cpp
SOURCES += gpio.cpp
SOURCES += adac.cpp
SOURCES += timer.cpp
SOURCES += wrap.cpp
SOURCES += ../../simple/llvm/PesApi.c

HEADERS  = mainwindow.h
HEADERS += DigitalIO.h
HEADERS += AnalogIO.h
HEADERS += Loop.h
HEADERS += wrap.h

HEADERS += ../inc/iogbl.h
HEADERS += ../inc/gpio.h
HEADERS += ../inc/adac.h
HEADERS += ../inc/timer.h

DESTDIR  = ../bin

FORMS    = pesim.ui

#LIBS     += -ldl

#RESOURCES += pesim.qrc

unix {
    MOC_DIR     = moc
    OBJECTS_DIR = obj
    DEFINES    += UNIX
}

win32 {
    MOC_DIR     = moc
    OBJECTS_DIR = obj
    DEFINES    += QT_DLL WIN32
}


