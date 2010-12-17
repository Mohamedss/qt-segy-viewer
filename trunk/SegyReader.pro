#-------------------------------------------------
#
# Project created by QtCreator 2010-12-01T08:27:50
#
#-------------------------------------------------

QT       += core gui

TARGET = SegyReader
TEMPLATE = app
RC_FILE = SegyReader.rc


SOURCES += main.cpp\
        mainwindow.cpp \
    util.cpp \
    segy.cpp \
    segyanalyzer.cpp \
    setbyte.cpp \
    printview.cpp \
    ilxlrange.cpp

HEADERS  += mainwindow.h \
    util.h \
    segy.h \
    segyanalyzer.h \
    setbyte.h \
    printview.h \
    ilxlrange.h

FORMS    += mainwindow.ui \
    segyanalyzer.ui \
    setbyte.ui \
    ilxlrange.ui

RESOURCES += \
    resources.qrc
