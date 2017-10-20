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
VPATH +=    ./src/algorithms \
            ./src/algorithms \
            ./src/network \
            ./src/widgets \
			./inc/algorithms \
            ./inc/algorithms \
            ./inc/network \
            ./inc/widgets 

INCLUDEPATH +=  ./inc \
                ./inc/algorithms \
                ./inc/network \
                ./inc/widgets 

SOURCES +=  main.cpp\
            SensorWindow.cpp \
            DragWidget.cpp \
            SensorWidget.cpp \
            ClusterWidget.cpp \
            NodeConfiguration.cpp \
            WidgetFactory.cpp

HEADERS  += SensorWindow.h \
            DragWidget.h \
            SensorWidget.h \
            ClusterWidget.h \
            SensorConfiguration.h \
            WidgetFactory.h \
            NodeConfiguration.h

RESOURCES += WSN.qrc
