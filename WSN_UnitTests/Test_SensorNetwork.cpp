#include "Test_SensorNetwork.h"
#include "SensorNetwork.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"


void Test_SensorNetwork::test_addLayer()
{
    SensorNetwork network;
    QCOMPARE(network.getNumberOfLayers(), 0);
    network.addLayer();
    QCOMPARE(network.getNumberOfLayers(), 1);
}

void Test_SensorNetwork::test_createNode()
{

}

void Test_SensorNetwork::test_connectNodes()
{

}

void Test_SensorNetwork::test_findBestPath()
{

}
