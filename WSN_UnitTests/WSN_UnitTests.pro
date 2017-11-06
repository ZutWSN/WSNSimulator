#-------------------------------------------------
#
# Project created by QtCreator 2017-10-14T11:09:45
#
#-------------------------------------------------

QT       += core gui
QT       += testlib

CONFIG += c++11
CONFIG += testcase

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WSN_UnitTests
TEMPLATE = app

VPATH += ../WSN/pkg \
         ../WSN/
VPATH +=    ../WSN/pkg/algorithms \
            ../WSN/pkg/algorithms \
           ../WSN/pkg/network \
            ../WSN/pkg/widgets

INCLUDEPATH +=  ../WSN/pkg \
                ../WSN/pkg/algorithms \
                ../WSN/pkg/network \
                ../WSN/pkg/widgets
#Later on don't add the sources just the headers, maybe just use dll or object files
SOURCES += \
    Test_NetworkNodes.cpp \
    SensorWindow.cpp \
    DragWidget.cpp \
    SensorWidget.cpp \
    ClusterWidget.cpp \
    NodeConfiguration.cpp \
    WidgetFactory.cpp \
    SensorNetwork.cpp \
    NetworkNode.cpp \
    SensorNode.cpp \
    ClusterNode.cpp \
    sinknode.cpp \
    NetworkLayer.cpp \
    DataFrame.cpp \
    SinkWidget.cpp \
    testmain.cpp \
    Test_Widgets.cpp \
    test_NetworkLayer.cpp \
    Test_SensorNetwork.cpp \
    Test_DataFrame.cpp \


HEADERS  += \
    SensorWindow.h \
    DragWidget.h \
    SensorWidget.h \
    ClusterWidget.h \
    WidgetFactory.h \
    NodeConfiguration.h \
    NetworkNode.h \
    SensorNode.h \
    ClusterNode.h \
    sinknode.h \
    NetworkLayer.h \
    DataFrame.h \
    SinkWidget.h \
    SensorNetwork.h \
    Test_NetworkNodes.h \
    Test_Widgets.h \
    test_NetworkLayer.h \
    Test_SensorNetwork.h \
    testconfig.h \
    Test_DataFrame.h \

RESOURCES += WSN.qrc
