#-------------------------------------------------
#
# Project created by QtCreator 2014-02-28T18:19:24
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chathack
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    client.cpp \
    program.cc

HEADERS  += mainwindow.h \
    client.h \
    statustype.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

#RC_FILE = chathack.rc

OTHER_FILES += \
    send2grid.py
