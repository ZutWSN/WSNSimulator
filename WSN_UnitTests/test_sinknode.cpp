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

void Test_SinkNode::test_receivedDataFromCluster()
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
    QCOMPARE(directSensor->connectToNode(directCluster), true);
    QCOMPARE(forwardSensor->connectToNode(forwardCluster), true);
    QCOMPARE(directCluster->connectToNode(forwardCluster), true);

    //connect directCluster to sink
    QCOMPARE(sink->addDirectCluster(directCluster), true);

    //register for signals and store count
    QSignalSpy sinkReceivedData(sink, SIGNAL(receivedData(DataFrame)));
    QSignalSpy directClusterSendDataToSink(directCluster, SIGNAL(sendDataToSink(DataFrame)));
    QSignalSpy directClusterSendDataToNeighbour(directCluster, SIGNAL(dataSend(DataFrame)));
    QSignalSpy forwardClusterSendData(forwardCluster, SIGNAL(dataSend(DataFrame)));

    //direct sensor sends data to its cluster and it forwards to sink
    QCOMPARE(directClusterSendDataToSink.count(), 0);
    directSensor->sendDataToCluster("Direct Sensor Data");
    QCOMPARE(directClusterSendDataToSink.count(), 1);
    QCOMPARE(sinkReceivedData.count(), 1);
    QVERIFY(sink->getLastMsg().getMsg() == "Direct Sensor Data");

    //forwardCLuster forwards data from its sensor to directCluster
    QCOMPARE(forwardClusterSendData.count(), 0);
    //get path to sink from directCluster neighbour
    forwardCluster->sendSinkPathReq();
    //1 send - path request, 2 - send after receiving path, 3- sends it to sink, sink
    //updates node cluster path
    QCOMPARE(forwardClusterSendData.count(), 3);
    //sends received msg acknowledge and sink path, then send acknowledge and forward new
    //sink path
    QCOMPARE(directClusterSendDataToNeighbour.count(), 3);
    QCOMPARE(directClusterSendDataToSink.count(), 2);
    forwardSensor->sendDataToCluster("Forward Sensor Data");
    QCOMPARE(forwardClusterSendData.count(), 4);
    QCOMPARE(directClusterSendDataToSink.count(), 3);
    QCOMPARE(sinkReceivedData.count(), 2);
    QVERIFY(sink->getLastMsg().getMsg() =="Forward Sensor Data");
}

//TDO
void Test_SinkNode::test_receivedNodeRemoved()
{
    SinkNode *sink = SinkNode::getSinkInstance();
    sink->setPosition(QPoint(0, 0));
    sink->setRange(20);
    qRegisterMetaType<DataFrame>();

    NetworkLayer layer(0);
    ClusterNode *directCluster = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 0));
    ClusterNode *forwardCluster1 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 1));
    ClusterNode *forwardCluster2 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 2));
    SensorNode *forwardSensor1 = static_cast<SensorNode*>(layer.createNode(NetworkNode::NodeType::Sensor, 3));
    SensorNode *forwardSensor2 = static_cast<SensorNode*>(layer.createNode(NetworkNode::NodeType::Sensor, 4));

    //set node parameters
    //clusters
    directCluster->setNodePosition(QPoint(0, 10));
    directCluster->setNodeRange(15);
    forwardCluster1->setNodePosition(QPoint(0, 25));
    forwardCluster1->setNodeRange(15);
    forwardCluster2->setNodePosition(QPoint(0, 39));
    forwardCluster2->setNodeRange(15);
    //sensors
    forwardSensor1->setNodePosition(QPoint(0, 29));
    forwardSensor1->setNodeRange(5);
    forwardSensor2->setNodePosition(QPoint(0, 42));
    forwardSensor2->setNodeRange(5);
    //connect clusters and sensors
    QCOMPARE(forwardSensor1->connectToNode(forwardCluster1), true);
    QCOMPARE(forwardSensor2->connectToNode(forwardCluster2), true);
    QCOMPARE(forwardCluster2->connectToNode(forwardCluster1), true);
    QCOMPARE(directCluster->connectToNode(forwardCluster1), true);
    QCOMPARE(sink->addDirectCluster(directCluster), true);
    QCOMPARE(sinkBroadcastSignal.count(), 0);
    //obtain sink paths for mapped clusters
    forwardCluster1->sendSinkPathReq();
    forwardCluster2->sendSinkPathReq();
    //check if clusters are mapped by sink
    QCOMPARE(sink->checkIfHasDirectCluster(directCluster), true);
    QCOMPARE(sink->checkIfHasMappedCluster(forwardCluster1->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink->checkIfHasMappedCluster(forwardCluster2->getNodeID(), layer.getLayerId()), true);
    //clusters in correct states
    QVERIFY(forwardCluster1->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster2->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(directCluster->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    //check if clusters have correct paths
    QVector<quint16> sinkPath1{0};
    QVector<quint16> sinkPath2{0, 1};
    QVERIFY(forwardCluster1->getSinkPath() == sinkPath1);
    QVERIFY(forwardCluster2->getSinkPath() == sinkPath2);
    //register for sink broadcasting signal
    QSignalSpy sinkBroadcastSignal(sink, SIGNAL(broadCastDataToClusters(DataFrame)));
    QCOMPARE(sinkBroadcastSignal.count(), 0);
    //remove a node from layer
    QCOMPARE(layer.removeNode(1), true);
    QCOMPARE(sinkBroadcastSignal.count(), 1);
    QCOMPARE(sink->checkIfHasMappedCluster(forwardCluster1->getNodeID(), layer.getLayerId()), false);
    //after that forwardCluster2 is an dangling cluster that doesnt have any neighbours
    //waits for new cluster in range to be added to reconnect with sink
    QVERIFY(forwardCluster2->getSinkPath().isEmpty());
    QVERIFY(forwardCluster2->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QCOMPARE(forwardCluster2->getNumOfConnectedNodes(), 0);
    //dangling sensor left
    QCOMPARE(forwardSensor1->isConnectedToCluster(), false);
    //create new cluster to replace the old one in forwarding the data to sink
    ClusterNode *forwardCluster3 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 5));
    forwardCluster3->setNodePosition(QPoint(0, 25));
    forwardCluster3->setNodeRange(15);
    QCOMPARE(directCluster->connectToNode(forwardCluster3), true);
    QCOMPARE(forwardSensor1->connectToNode(forwardCluster3), true);
    //connect dangling cluster again to have it connected to sink
    QCOMPARE(forwardCluster2->connectToNode(forwardCluster3), true);
    forwardCluster1->sendSinkPathReq();
    QVERIFY(forwardCluster2->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
}

void Test_SinkNode::test_receivedClusterPath()
{

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
