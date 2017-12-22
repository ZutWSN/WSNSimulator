#include "Test_SensorNetwork.h"
#include "SensorNetwork.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"

void Test_SensorNetwork::test_addLayer()
{
    SensorNetwork network;
    QVERIFY(network.getNumberOfLayers() == 0);
    network.addLayer(0);
    QVERIFY(network.getNumberOfLayers() == 1);
}

void Test_SensorNetwork::test_getLayer()
{
    SensorNetwork network;
    network.addLayer(0);
    QVERIFY(network.getLayer(0) != nullptr);
    QVERIFY(network.getLayer(1) == nullptr);
}

void Test_SensorNetwork::test_checkIfHasLayer()
{

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

void Test_SensorNetwork::test_connectSensorToNearestCluster()
{

}

