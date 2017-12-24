#include "Test_SensorNetwork.h"
#include "SensorNetwork.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"

void Test_SensorNetwork::test_onSinkAdded()
{
    //check if network already has sink and if it has not
    //add it at specific position
    SensorNetwork network;
    //connect sink added signal - drag'ndrop sink widget simulation
    connect(this, SIGNAL(addNewSink(QPoint,quint16)), &network, SLOT(onSinkAdded(QPoint,quint16)));
    QCOMPARE(network.networkHasSink(), false);
    emit addNewSink(QPoint(0, 0), 10);
    QCOMPARE(network.networkHasSink(), true);
}

void Test_SensorNetwork::test_onNewClusterAdded()
{
    //connect it to all neighbour clusters -> clusters in range
    //go through all of them and connect only to those in range
    //then get sink path
    //and send it to sink to update it network mapping
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

