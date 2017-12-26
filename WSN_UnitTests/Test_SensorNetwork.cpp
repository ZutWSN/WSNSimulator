#include "Test_SensorNetwork.h"
#include "SensorNetwork.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"
#include "widgetfactory.h"
#include "ClusterNode.h"
#include "SensorNode.h"
#include <QPair>

void Test_SensorNetwork::initTestCase()
{
    network.reset(new SensorNetwork);
    //connect all signals to network object
    connect(this, SIGNAL(addNewSink(QPoint,quint16,QWidget*)), network.data(), SLOT(onSinkAdded(QPoint,quint16,QWidget*)));
    connect(this, SIGNAL(addNewCluster(quint16,quint16,QPoint,quint16,QWidget*)), network.data(), SLOT(onNewClusterAdded(quint16,quint16,QPoint,quint16,QWidget*)));
    connect(this, SIGNAL(addNewSensor(quint16,quint16,QPoint,quint16,QWidget*)), network.data(), SLOT(onNewSensorAdded(quint16,quint16,QPoint,quint16,QWidget*)));
    connect(this, SIGNAL(removeSink()), network.data(), SLOT(onSinkRemoved()));
    connect(this, SIGNAL(removeCluster(quint16,quint16)), network.data(), SLOT(onClusterRemoved(quint16,quint16)));
    connect(this, SIGNAL(removeSensor(quint16,quint16)), network.data(), SLOT(onSensorRemoved(quint16,quint16)));

    WidgetFactory wFactory;
    sensorWgt0.reset(static_cast<QWidget*>(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor)));
    sensorWgt1.reset(static_cast<QWidget*>(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor)));
    clusterWgt0.reset(static_cast<QWidget*>(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster)));
    clusterWgt1.reset(static_cast<QWidget*>(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster)));
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
    emit addNewCluster(0, 0, QPoint(0, 5), 10, clusterWgt0.data());
    emit addNewCluster(1, 0, QPoint(0, 10), 10, clusterWgt1.data());
    QVERIFY(network->getNumberOfLayers() == 1);
    auto layer = network->getLayer(0);
    QVERIFY(layer != nullptr);
    QVERIFY(layer->getNumOfNodes() == 2);
    QVERIFY(layer->getNode(0) != nullptr);
    QVERIFY(layer->getNode(1) != nullptr);
    //check cluster states and if connected to sink
    ClusterNode *cluster0 = static_cast<ClusterNode*>(layer->getNode(0));
    ClusterNode *cluster1 = static_cast<ClusterNode*>(layer->getNode(1));
    QCOMPARE(cluster0->getCurrentState(), ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    QCOMPARE(cluster1->getCurrentState(), ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    QCOMPARE(cluster0->checkIfConnectedToNode(cluster1), true);
}

void Test_SensorNetwork::test_onNewSensorAdded()
{
    //connect to nearest cluster in range
    //if not clusters in range set as dangling disconnected sensor
    emit addNewSensor(2, 0, QPoint(0, 12), 8, sensorWgt0.data());
    emit addNewSensor(3, 0, QPoint(0, 6), 8, sensorWgt1.data());
    NetworkLayer *layer = network->getLayer(0);
    QVERIFY(layer->getNumOfNodes() == 4);
    SensorNode *sensor0 = static_cast<SensorNode*>(layer->getNode(2));
    SensorNode *sensor1 = static_cast<SensorNode*>(layer->getNode(3));
    QVERIFY(sensor0 != nullptr);
    QVERIFY(sensor1 != nullptr);
    QVERIFY(sensor0->isConnectedToCluster());
    QVERIFY(sensor1->isConnectedToCluster());
    QVERIFY(sensor0->getClusterID() == 1);
    QVERIFY(sensor1->getClusterID() == 0);
}

void Test_SensorNetwork::test_onSinkRemoved()
{
   QVERIFY(network->networkHasSink());
   emit removeSink();
   QVERIFY(!network->networkHasSink());
   auto layer = network->getLayer(0);
   //check if all node states are disconnected
   quint16 i = 0;
   for(QVector<NetworkNode*>::const_iterator node = layer->getIteratorToFirstNode(); i < layer->getNumOfNodes(); node++)
   {
       i++;
       if((*node)->getNodeType() == NetworkNode::NodeType::Cluster)
       {
           auto cluster = static_cast<ClusterNode*>(*node);
           QCOMPARE(cluster->getCurrentState(), ClusterNode::ClusterStates::DISCONNECTED);
       }
   }
}

void Test_SensorNetwork::test_onNewClusterRemoved()
{
    //add sink again
    emit addNewSink(QPoint(0, 0), 10, sinkWgt.data());
    auto layer = network->getLayer(0);
    //check if all clusters are connected again
    quint16 i = 0;
    for(QVector<NetworkNode*>::const_iterator node = layer->getIteratorToFirstNode(); i < layer->getNumOfNodes(); node++)
    {
        i++;
        if((*node)->getNodeType() == NetworkNode::NodeType::Cluster)
        {
            auto cluster = static_cast<ClusterNode*>(*node);
            QCOMPARE(cluster->getCurrentState(), ClusterNode::ClusterStates::CONNECTED_TO_SINK);
        }
    }
    //remove one of the clusters
    auto cluster = static_cast<ClusterNode*>(layer->getNode(1));
    QVERIFY(layer->getNumOfNodes() == 4);
    QVERIFY(cluster->getNumOfSensors() == 1);
    emit removeCluster(0, 0);
    QVERIFY(layer->getNumOfNodes() == 3);
    QVERIFY(layer->getNode(0) == nullptr);
    //sensor reconnects with nearest cluster
    QVERIFY(cluster->getNumOfSensors() == 2);
    QVERIFY(!cluster->checkIfConnectedToNode(qMakePair(0, 0)));
}

void Test_SensorNetwork::test_onNewSensorRemoved()
{
    auto layer = network->getLayer(0);
    auto cluster = static_cast<ClusterNode*>(layer->getNode(1));
    QVERIFY(layer->getNumOfNodes() == 3);
    QVERIFY(cluster->getNumOfSensors() == 2);
    emit removeSensor(2, 0);
    QVERIFY(cluster->getNumOfSensors() == 1);
    QVERIFY(layer->getNumOfNodes() == 2);
}

void Test_SensorNetwork::cleanupTestCase()
{
    disconnect(this, 0, network.data(), 0);
}

