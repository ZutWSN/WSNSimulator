#-------------------------------------------------
#
# Project created by QtCreator 2017-10-14T11:09:45
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WSN
TEMPLATE = app

VPATH += ./pkg
VPATH +=    ./pkg/algorithms \
            ./pkg/algorithms \
            ./pkg/network \
            ./pkg/widgets

INCLUDEPATH +=  ./pkg \
                ./pkg/algorithms \
                ./pkg/network \
                ./pkg/widgets

SOURCES +=  main.cpp\
            SensorWindow.cpp \
            DragWidget.cpp \
            SensorWidget.cpp \
            ClusterWidget.cpp \
            NodeConfiguration.cpp \
            WidgetFactory.cpp \
            NetworkNode.cpp \
    pkg/network/SensorNode.cpp \
    pkg/network/ClusterNode.cpp \
    pkg/network/sinknode.cpp \
    pkg/network/NetworkLayer.cpp \
    pkg/network/DataFrame.cpp \
    pkg/widgets/SinkWidget.cpp \
    pkg/network/SensorNetwork.cpp \
    pkg/network/NetworkManager.cpp

HEADERS  += SensorWindow.h \
            DragWidget.h \
            SensorWidget.h \
            ClusterWidget.h \
            WidgetFactory.h \
            NodeConfiguration.h \
            NetworkNode.h \
    pkg/network/SensorNode.h \
    pkg/network/ClusterNode.h \
    pkg/network/sinknode.h \
    pkg/network/NetworkLayer.h \
    pkg/network/DataFrame.h \
    pkg/widgets/SinkWidget.h \
    pkg/network/SensorNetwork.h \
    pkg/network/NetworkManager.h

RESOURCES += WSN.qrc
