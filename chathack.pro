#-------------------------------------------------
#
# Project created by QtCreator 2014-02-28T20:12:47
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = chathack
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    logwriter.cpp \
    worker.cpp

HEADERS += \
    server.h \
    logwriter.h \
    worker.h
