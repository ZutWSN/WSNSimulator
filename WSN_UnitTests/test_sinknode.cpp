#include "test_sinknode.h"
#include "sinknode.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"
#include "ClusterNode.h"
#include "SensorNode.h"

void Test_SinkNode::test_getSinkInstance()
{
    SinkNode *sink = SinkNode::getSinkInstance();
    sink->setPosition(QPoint(0, 0));
    sink->setRange(20);
    QVERIFY(sink != nullptr);
    QCOMPARE(SinkNode::getSinkInstance(), sink);
}

void Test_SinkNode::test_addRemoveDirectCluster()
{
    SinkNode *sink = SinkNode::getSinkInstance();
    sink->setPosition(QPoint(0, 0));
    sink->setRange(20);
    NetworkLayer layer(0);
    NetworkNode *cluster1 = layer.createNode(NetworkNode::NodeType::Cluster, 0);
    NetworkNode *cluster2 = layer.createNode(NetworkNode::NodeType::Cluster, 1);
    cluster1->setNodePosition(QPoint(0, 10));
    cluster1->setNodeRange(15);
    cluster2->setNodePosition(QPoint(0, 21));
    cluster2->setNodeRange(20);
    QCOMPARE(sink->addDirectCluster(cluster1), true);
    QCOMPARE(sink->addDirectCluster(cluster2), false);
    QCOMPARE(sink->removeDirectCluster(cluster1), true);
    QCOMPARE(sink->removeDirectCluster(cluster2), false);
}

void Test_SinkNode::test_onReceivedDataFromCluster()
{
    SinkNode *sink = SinkNode::getSinkInstance();
    sink->setPosition(QPoint(0, 0));
    sink->setRange(20);
    qRegisterMetaType<DataFrame>();

    //create nodes
    NetworkLayer layer(0);
    ClusterNode *directCluster = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 0));
    ClusterNode *forwardCluster = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 1));
    SensorNode *directSensor = static_cast<SensorNode*>(layer.createNode(NetworkNode::NodeType::Sensor, 2));
    SensorNode *forwardSensor = static_cast<SensorNode*>(layer.createNode(NetworkNode::NodeType::Sensor, 3));

    //set node parameters
    //clusters
    directCluster->setNodePosition(QPoint(0, 10));
    directCluster->setNodeRange(15);
    forwardCluster->setNodePosition(QPoint(0, 25));
    forwardCluster->setNodeRange(15);
    //sensors
    directSensor->setNodePosition(QPoint(0, 12));
    directSensor->setNodeRange(5);
    forwardSensor->setNodePosition(QPoint(0, 28));
    forwardSensor->setNodeRange(5);
    //connect clusters and sensors
    directSensor->connectToNode(directCluster);
    forwardSensor->connectToNode(forwardCluster);
    directCluster->connectToNode(forwardCluster);

    //connect directCluster to sink
    QCOMPARE(sink->addDirectCluster(directCluster), true);

    //register for signals and store count
    QSignalSpy sinkReceivedData(sink, SIGNAL(receivedData(DataFrame)));
    QSignalSpy directClusterSendData(directCluster, SIGNAL(sendDataToSink(DataFrame)));
    QSignalSpy forwardClusterSendData(forwardCluster, SIGNAL(dataSend(DataFrame)));

    //direct sensor sends data to its cluster and it forwards to sink
    QCOMPARE(directClusterSendData.count(), 0);
    directSensor->sendDataToCluster("Direct Sensor Data");
    QCOMPARE(directClusterSendData.count(), 1);
    QCOMPARE(sinkReceivedData.count(), 1);
    QVERIFY(sink->getLastMsg().getMsg() == "Direct Sensor Data");

    //forwardCLuster forwards data from its sensor to directCluster
    QCOMPARE(forwardClusterSendData.count(), 0);
    //get path to sink from directCluster neighbour
    forwardCluster->sendSinkPathReq();
    //1 send - path request, 2 - send after receiving path, sends it to sink, sink
    //updates node cluster path
    QCOMPARE(forwardClusterSendData.count(), 2);
    QCOMPARE(directClusterSendData.count(), 2);
    forwardSensor->sendDataToCluster("Forward Sensor Data");
    QCOMPARE(forwardClusterSendData.count(), 3);
    QCOMPARE(directClusterSendData.count(), 3);
    QCOMPARE(sinkReceivedData.count(), 2);
    QVERIFY(sink->getLastMsg().getMsg() =="Forward Sensor Data");
}

void Test_SinkNode::test_sendNewPaths()
{
    SinkNode *sink = SinkNode::getSinkInstance();
    sink->setPosition(QPoint(0, 0));
    sink->setRange(20);
}
/*
 * private methods, test using test_onReceivedDataFromCluster test case
void Test_SinkNode::test_broadCastDataToClusters()
{

}

void Test_SinkNode::test_calculateNetworkPaths()
{

}

void Test_SinkNode::test_updateClusterPath()
{

}
*/
