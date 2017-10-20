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
    SensorWindow.cpp \
    DragWidget.cpp \
    SensorWidget.cpp \
    ClusterWidget.cpp \
    NodeConfiguration.cpp \
    WidgetFactory.cpp

HEADERS  += \
    SensorWindow.h \
    DragWidget.h \
    SensorWidget.h \
    ClusterWidget.h \
    SensorConfiguration.h \
    WidgetFactory.h \
    NodeConfiguration.h

RESOURCES += WSN.qrc
