#-------------------------------------------------
#
# Project created by QtCreator 2014-02-28T20:12:47
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = chathack
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#LIBS += -L/usr/local/mysql-5.5.36-osx10.6-x86_64/lib -lmysqlclient
#INCLUDE += /usr/local/mysql-5.5.36-osx10.6-x86_64/include

SOURCES += main.cpp \
    server.cpp \
    logwriter.cpp \
    database.cpp \
    worker.cpp

HEADERS += \
    server.h \
    logwriter.h \
    database.h \
    worker.h
