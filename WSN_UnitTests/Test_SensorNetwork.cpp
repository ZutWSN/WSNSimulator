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

}

void Test_SensorNetwork::test_onNewSensorAdded()
{

}

void Test_SensorNetwork::test_connectSensorToNearestCluster()
{

}

void Test_SensorNetwork::test_synchronizeNetwork()
{

}

void Test_SensorNetwork::test_calculateNetworkPaths()
{

}

