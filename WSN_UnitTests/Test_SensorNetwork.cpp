#include "Test_SensorNetwork.h"
#include "SensorNetwork.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"
#include "widgetfactory.h"

void Test_SensorNetwork::initTestCase()
{
    network.reset(new SensorNetwork);
    //connect all signals to network object
    connect(this, SIGNAL(addNewSink(QPoint,quint16,QWidget*)), network.data(), SLOT(onSinkAdded(QPoint,quint16,QWidget*)));
    connect(this, SIGNAL(addNewCluster(quint16,quint16,QPoint,quint16,QWidget*)), network.data(), SLOT(onNewClusterAdded(quint16,quint16,QPoint,quint16,QWidget*)));
    connect(this, SIGNAL(addNewSensor(quint16,quint16,QPoint,quint16,QWidget*)), network.data(), SLOT(onNewSensorAdded(quint16,quint16,QPoint,quint16,QWidget*)));

    WidgetFactory wFactory;
    sensorWgt.reset(static_cast<QWidget*>(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor)));
    clusterWgt.reset(static_cast<QWidget*>(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster)));
    sinkWgt.reset(static_cast<QWidget*>(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sink)));
}

void Test_SensorNetwork::test_onSinkAdded()
{
    //check if network already has sink and if it has not
    //add it at specific position
    //connect sink added signal - drag'ndrop sink widget simulation
    QCOMPARE(network->networkHasSink(), false);
    emit addNewSink(QPoint(0, 0), 10, sinkWgt.data());
    QCOMPARE(network->networkHasSink(), true);
}

void Test_SensorNetwork::test_onNewClusterAdded()
{
    //connect it to all neighbour clusters -> clusters in range
    //go through all of them and connect only to those in range
    //then get sink path
    //and send it to sink to update it network mapping
    QVERIFY(network->getNumberOfLayers() == 0);
    emit addNewCluster(0, 0, QPoint(0, 5), 10, clusterWgt.data());
    emit addNewCluster(1, 0, QPoint(0, 10), 10, clusterWgt.data());
    QVERIFY(network->getNumberOfLayers() == 1);
    auto layer = network->getLayer(0);
    QVERIFY(layer != nullptr);
    QVERIFY(layer->getNumOfNodes() == 2);
    QVERIFY(layer->getNode(0) != nullptr);
    QVERIFY(layer->getNode(1) != nullptr);
    //check cluster states and if connected to sink
}

void Test_SensorNetwork::test_onNewSensorAdded()
{
    //connect to nearest cluster in range
    //if not clusters in range set as dangling disconnected sensor
}

void Test_SensorNetwork::test_onSinkRemoved()
{

}

void Test_SensorNetwork::test_onNewClusterRemoved()
{

}

void Test_SensorNetwork::test_onNewSensorRemoved()
{

}

void Test_SensorNetwork::cleanupTestCase()
{
    disconnect(this, 0, network.data(), 0);
}

