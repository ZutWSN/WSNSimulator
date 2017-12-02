#include "Test_ClusterNode.h"
#include "ClusterNode.h"
#include "SensorNode.h"
#include "sinknode.h"
#include <QDebug>

void Test_ClusterNode::test_connectToNode()
{
    ClusterNode cluster1(0, 15, 0, QPoint(0, 0));
    ClusterNode cluster2(1, 20, 0, QPoint(10, 10));
    SensorNode sensor1(2, 2, 0, QPoint(1, 1));
    SensorNode sensor2(3, 4, 0, QPoint(12, 12));
    //connect to other cluster
    QCOMPARE(cluster1.connectToNode(&cluster2), true);
    QCOMPARE(cluster1.checkIfConnectedToNode(&cluster2), true);
    //connect sensor to first cluster
    QCOMPARE(cluster1.connectToNode(&sensor1), true);
    QCOMPARE(cluster1.checkIfConnectedToSensor(&sensor1), true);
    //connect second sensor to cluster - error not in range
    QCOMPARE(cluster1.connectToNode(&sensor2), false);
    //connect seconf sensor to second cluster
    QCOMPARE(cluster2.connectToNode(&sensor2), true);
}

void Test_ClusterNode::test_disconnectFromNode()
{
    ClusterNode cluster1(0, 15, 0, QPoint(0, 0));
    ClusterNode cluster2(1, 20, 0, QPoint(10, 10));
    SensorNode sensor(2, 2, 0, QPoint(1, 1));
    cluster1.connectToNode(&cluster2);
    //can't disconnect from itself
    QCOMPARE(cluster1.disconnectFromNode(&cluster1), false);
    //can disconnect from cluster2
    QCOMPARE(cluster1.disconnectFromNode(&cluster2), true);
    //cant disconnect from sensor, not connected
    QCOMPARE(cluster1.disconnectFromNode(&sensor), false);
    cluster1.connectToNode(&sensor);
    QCOMPARE(cluster1.disconnectFromNode(&sensor), true);
}

void Test_ClusterNode::test_getNumOfSensors()
{
    ClusterNode cluster(0, 15, 0, QPoint(0, 0));
    SensorNode sensor1(2, 2, 0, QPoint(1, 1));
    SensorNode sensor2(3, 4, 0, QPoint(2, 1));
    SensorNode sensor3(4, 2, 0, QPoint(0, 16));
    SensorNode sensor4(5, 4, 1, QPoint(1, 2));
    cluster.connectToNode(&sensor1);
    cluster.connectToNode(&sensor2);
    cluster.connectToNode(&sensor3);
    cluster.connectToNode(&sensor4);
    QVERIFY(cluster.getNumOfSensors() == 2);
}

void Test_ClusterNode::test_checkIfConnectedToSensor()
{
    ClusterNode cluster(0, 15, 0, QPoint(0, 0));
    SensorNode sensor1(2, 2, 0, QPoint(1, 1));
    SensorNode sensor2(4, 2, 0, QPoint(0, 16));
    cluster.connectToNode(&sensor1);
    cluster.connectToNode(&sensor2);
    QCOMPARE(cluster.checkIfConnectedToSensor(&sensor1), true);
    QCOMPARE(cluster.checkIfConnectedToSensor(&sensor2), false);
}


void Test_ClusterNode::test_onReceivedDataFromSensor()
{
    ClusterNode cluster(0, 15, 0, QPoint(0, 0));
    SensorNode sensor1(2, 2, 0, QPoint(1, 1));
    SensorNode sensor2(3, 2, 0, QPoint(1, 0));
    cluster.setSinkPath({1, 2});
    qRegisterMetaType<DataFrame>();

    cluster.connectToNode(&sensor1);
    cluster.connectToNode(&sensor2);
    //register for signals and store count
    QSignalSpy sensor1SendData(&sensor1, SIGNAL(clusterDataSend(QByteArray)));
    QSignalSpy sensor2SendData(&sensor2, SIGNAL(clusterDataSend(QByteArray)));
    QSignalSpy clusterSendData(&cluster, SIGNAL(dataSend(DataFrame)));
    //check if everything wasn't called yet
    QCOMPARE(sensor1SendData.count(), 0);
    QCOMPARE(sensor2SendData.count(), 0);
    QCOMPARE(clusterSendData.count(), 0);

    sensor1.sendDataToCluster("Hello from Sensor1!");
    QCOMPARE(sensor1SendData.count(), 1);
    QCOMPARE(clusterSendData.count(), 0);
    QVERIFY(cluster.getNumOfSensorPendingMsgs() == 1);
    sensor2.sendDataToCluster("Hello from Sensor2!");
    QCOMPARE(sensor2SendData.count(), 1);
    QCOMPARE(clusterSendData.count(), 1);
    QVERIFY(cluster.getNumOfSensorPendingMsgs() == 0);
}


void Test_ClusterNode::test_onReceivedData()
{
    //after created sink design - needs sensor network and
    //layer and sink to be used
    //This test will showcase main functionality if the network
    //message routing and path seeking
}

void Test_ClusterNode::test_sendSinkPathReq()
{
    ClusterNode cluster(0, 15, 0, QPoint(0, 0));
    ClusterNode neighbourCluster(1, 15, 0, QPoint(10, 10));
    cluster.setSinkPath({4, 5});
    cluster.setPathLength(0);
    QVector<quint16> new_path = {2, 3};
    neighbourCluster.setSinkPath(new_path);
    neighbourCluster.setPathLength(7);
    qRegisterMetaType<DataFrame>();

    cluster.connectToNode(&neighbourCluster);

    QSignalSpy clusterSendData(&cluster, SIGNAL(dataSend(DataFrame)));
    QSignalSpy neighbourClusterSendData(&neighbourCluster, SIGNAL(dataSend(DataFrame)));
    //rxreceived msg, neighbour path, forward new path  - 3 sends
    //send request for path, send path received, send new sink path to sink - 3 sends
    QCOMPARE(cluster.sendSinkPathReq(), true);
    QCOMPARE(clusterSendData.count(), 3);
    QCOMPARE(neighbourClusterSendData.count(), 3);

    new_path.insert(new_path.begin(), neighbourCluster.getNodeID());
    QVERIFY(cluster.getSinkPath() == new_path);
}
