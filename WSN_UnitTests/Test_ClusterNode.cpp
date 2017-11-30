#include "Test_ClusterNode.h"
#include "ClusterNode.h"
#include "SensorNode.h"


void Test_ClusterNode::test_connectToNode()
{
    ClusterNode cluster1(0, 10, 0, QPoint(0, 0));
    ClusterNode cluster2(1, 20, 0, QPoint(5, 5));
    SensorNode sensor1(2, 2, 0, QPoint(1, 1));
    SensorNode sensor2(3, 2, 0, QPoint(12, 12));
    //connect to other cluster
    QCOMPARE(cluster1.connectToNode(static_cast<NetworkNode*>(&cluster2)), true);
    QCOMPARE(cluster1.checkIfConnectedToNode(static_cast<NetworkNode*>(&cluster2)), true);
    //connect sensor to first cluster
    QCOMPARE(cluster1.connectToNode(static_cast<NetworkNode*>(&sensor1)), true);
    QCOMPARE(cluster1.checkIfConnectedToSensor(static_cast<NetworkNode*>(&sensor1)), true);
    //connect second sensor to cluster - error not in range
    QCOMPARE(cluster1.connectToNode(static_cast<NetworkNode*>(&sensor2)), false);
    //connect seconf sensor to second cluster
    QCOMPARE(cluster2.connectToNode(static_cast<NetworkNode*>(&sensor2)), true);
}

void Test_ClusterNode::test_sendSinkPathReq()
{

}

void Test_ClusterNode::test_disconnectFromNode()
{

}

void Test_ClusterNode::test_getNumOfSensors()
{

}

void Test_ClusterNode::test_checkIfConnectedToSensor()
{

}

void Test_ClusterNode::test_getNodeType()
{

}

void Test_ClusterNode::test_onReceivedDataFromSensor()
{

}
