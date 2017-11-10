#include "Test_SensorNode.h"
#include "SensorNode.h"
#include "ClusterNode.h"

void Test_SensorNode::test_connectToCluster()
{
    SensorNode sensor(0);
    ClusterNode cluster(1);
    sensor.setLayer(0);
    cluster.setLayer(0);
    //connect to cluster on the same layer
    QCOMPARE(sensor.connectToNode(&cluster), true);
    SensorNode sensor2(2);
    sensor2.setLayer(0);
    //can't connect directly to sensor
    QCOMPARE(sensor.connectToNode(&sensor2), false);
    Cluster cluster2(3);
    cluster.setLayer(0);
    //fails because already connected to other target
    QCOMPARE(sensor.connectToNode(&cluster2), false);
}

void Test_SensorNode::test_disconnectFromCluster()
{
    SensorNode sensor(0);
    ClusterNode cluster(1);
    ClusterNode cluster2(2);
    sensor.setLayer(0);
    cluster.setLayer(0);
    cluster2.setLayer(0);
    //can't disconnect before connected
    QCOMPARE(sensor.disconnectFromNode(&cluster), false);
    sensor.connectToNode(&cluster);
    //cant disconnect , not valid node
    QCOMPARE(sensor.disconnectFromNode(&cluster2), false);
    //disconnect
    QCOMPARE(sensor.disconnectFromNode(&cluster), true);
}
