#include "Test_SensorNetwork.h"
#include "SensorNetwork.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"

void Test_SensorNetwork::test_addLayer()
{
    SensorNetwork network;
    QCOMPARE(network.getNumberOfLayers(), static_cast<quint16>(0));
    network.addLayer(0);
    QCOMPARE(network.getNumberOfLayers(), static_cast<quint16>(1));
}

void Test_SensorNetwork::test_getLayer()
{
    SensorNetwork network;
    network.addLayer(0);
    QVERIFY(network.getLayer(0) != nullptr);
    QCOMPARE(network.getLayer(1), nullptr);
}

