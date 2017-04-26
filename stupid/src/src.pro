QT     += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = stupid
TEMPLATE = app
CONFIG += warn_on \
          thread \
          qt
QT += network

INCLUDEPATH += ./view
INCLUDEPATH += ./view/comm
INCLUDEPATH += ./load

SOURCES += highlighter.cpp
SOURCES += mainwindow.cpp
SOURCES += codeeditor.cpp
SOURCES += main.cpp
SOURCES += DisplayError.cpp

SOURCES += ./view/platform.cpp
SOURCES += ./view/viewwindow.cpp
SOURCES += ./view/pesitem.cpp
SOURCES += ./view/inputdialog.cpp
SOURCES += ./view/watchelement.cpp
SOURCES += ./view/watchloop.cpp
SOURCES += ./view/DnlItem.cpp
SOURCES += ./view/comm/PesNet.cpp
SOURCES += ./view/comm/USBPes.cpp
SOURCES += ./view/comm/cudppes.cpp

SOURCES += ./load/Loader.cpp

HEADERS += highlighter.h
HEADERS += mainwindow.h
HEADERS += codeeditor.h
HEADERS += DisplayError.h

HEADERS += ./view/viewwindow.h
HEADERS += ./view/platform.h
HEADERS += ./view/pesitem.h
HEADERS += ./view/inputdialog.h
HEADERS += ./view/watchelement.h
HEADERS += ./view/watchloop.h
HEADERS += ./view/DnlItem.h
#HEADERS += cpes.h
HEADERS += ./view/comm/PesNet.h
HEADERS += ./view/comm/USBPes.h
HEADERS += ./view/comm/cudppes.h

HEADERS += ./load/Loader.h

#INCLUDEPATH += ./pesim/inc/

DESTDIR = ../bin

RESOURCES += ./view/viewer.qrc


FORMS+= xicht.ui
FORMS+= ./view/viewwindow.ui
FORMS+= ./view/inputdialog.ui
FORMS+= ./view/bit8dialog.ui
FORMS+= ./view/finddialog.ui
FORMS+= ./pesim/pesim.ui


LIBS = -L../posix/lib -lpesim -lstpapi -lasmapi
LIBS+= -lusb -ldl

#INCLUDEPATH += ./comm

DESTDIR = ../bin

unix {
    MOC_DIR     = moc
    OBJECTS_DIR = obj
    DEFINES    += UNIX
}

win32 {
    MOC_DIR     = moc
    OBJECTS_DIR = obj
    DEFINES    += QT_DLL WIN32
    LIBS += -lmsvcrt
}


