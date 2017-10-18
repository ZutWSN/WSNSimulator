#-------------------------------------------------
#
# Project created by QtCreator 2017-10-14T11:09:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WSN
TEMPLATE = app

VPATH += ./src \
         ./inc
INCLUDEPATH += ./inc
SOURCES += main.cpp\
    sensorwindow.cpp \
    dragwidget.cpp \
    sensorwidget.cpp \
    clusterwidget.cpp \
    sensorconfiguration.cpp

HEADERS  += \
    sensorwindow.h \
    dragwidget.h \
    sensorwidget.h \
    clusterwidget.h \
    sensorconfiguration.h
