#include "test_sinknode.h"
#include "sinknode.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"
#include "ClusterNode.h"
#include "SensorNode.h"

void Test_SinkNode::test_addRemoveDirectCluster()
{
    SinkNode sink;
    sink.setPosition(QPoint(0, 0));
    sink.setRange(20);
    NetworkLayer layer(0);
    NetworkNode *cluster1 = layer.createNode(NetworkNode::NodeType::Cluster, 0);
    NetworkNode *cluster2 = layer.createNode(NetworkNode::NodeType::Cluster, 1);
    cluster1->setNodePosition(QPoint(0, 10));
    cluster1->setNodeRange(15);
    cluster2->setNodePosition(QPoint(0, 21));
    cluster2->setNodeRange(20);
    QCOMPARE(sink.addDirectCluster(cluster1), true);
    QCOMPARE(sink.addDirectCluster(cluster2), false);
    QCOMPARE(sink.removeDirectCluster(cluster1), true);
    QCOMPARE(sink.removeDirectCluster(cluster2), false);
}

void Test_SinkNode::test_receivedDataFromCluster()
{
    SinkNode sink;
    sink.setPosition(QPoint(0, 0));
    sink.setRange(20);
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
    QCOMPARE(sink.addDirectCluster(directCluster), true);

    //register for signals and store count
    QSignalSpy sinkReceivedData(&sink, SIGNAL(receivedData(DataFrame)));
    QSignalSpy directClusterSendDataToSink(directCluster, SIGNAL(sendDataToSink(DataFrame)));
    QSignalSpy directClusterSendDataToNeighbour(directCluster, SIGNAL(dataSend(DataFrame)));
    QSignalSpy forwardClusterSendData(forwardCluster, SIGNAL(dataSend(DataFrame)));

    //direct sensor sends data to its cluster and it forwards to sink
    QCOMPARE(directClusterSendDataToSink.count(), 0);
    directSensor->sendDataToCluster("Direct Sensor Data");
    QCOMPARE(directClusterSendDataToSink.count(), 1);
    QCOMPARE(sinkReceivedData.count(), 1);
    QVERIFY(sink.getLastMsg().getMsg() == "Direct Sensor Data");

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
    QVERIFY(sink.getLastMsg().getMsg() =="Forward Sensor Data");
}

void Test_SinkNode::test_onReceivedNetworkChange()
{
    SinkNode sink;
    sink.setPosition(QPoint(10, 10));
    sink.setRange(4);
    qRegisterMetaType<DataFrame>();

    NetworkLayer layer(0);
    ClusterNode *directCluster1 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 1));
    ClusterNode *directCluster2 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 2));
    ClusterNode *directCluster3 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 3));
    ClusterNode *forwardCluster1 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 4));
    ClusterNode *forwardCluster2 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 5));
    ClusterNode *forwardCluster3 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 6));
    ClusterNode *forwardCluster4 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 7));
    ClusterNode *forwardCluster5 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 8));

    //set node parameters
    //direct clusters
    directCluster1->setNodePosition(QPoint(10, 7));
    directCluster1->setNodeRange(15);
    directCluster2->setNodePosition(QPoint(10, 14));
    directCluster2->setNodeRange(15);
    directCluster3->setNodePosition(QPoint(12, 10));
    directCluster3->setNodeRange(15);
    //mapped clusters
    forwardCluster1->setNodePosition(QPoint(11, 7));
    forwardCluster1->setNodeRange(15);
    forwardCluster2->setNodePosition(QPoint(10, 5));
    forwardCluster2->setNodeRange(15);
    forwardCluster3->setNodePosition(QPoint(11, 5));
    forwardCluster3->setNodeRange(15);
    forwardCluster4->setNodePosition(QPoint(16, 10));
    forwardCluster4->setNodeRange(15);
    forwardCluster5->setNodePosition(QPoint(12, 15));
    forwardCluster5->setNodeRange(15);
    //Connecting cluster nodes-------------------------------------
    //connect mapped clusters
    //C1
    QCOMPARE(forwardCluster1->connectToNode(forwardCluster2), true);
    QCOMPARE(forwardCluster1->connectToNode(forwardCluster3), true);
    QCOMPARE(forwardCluster1->connectToNode(directCluster1), true);
    //C2
    QCOMPARE(forwardCluster2->connectToNode(forwardCluster3), true);
    QCOMPARE(forwardCluster2->connectToNode(directCluster1), true);
    QCOMPARE(forwardCluster2->connectToNode(directCluster2), true);
    //C3
    QCOMPARE(forwardCluster3->connectToNode(directCluster2), true);
    //C4
    QCOMPARE(forwardCluster4->connectToNode(directCluster1), true);
    QCOMPARE(forwardCluster4->connectToNode(directCluster3), true);
    //C5
    QCOMPARE(forwardCluster5->connectToNode(directCluster2), true);
    QCOMPARE(forwardCluster5->connectToNode(directCluster3), true);
    //connect direct clusters to sink
    QCOMPARE(sink.addDirectCluster(directCluster1), true);
    QCOMPARE(sink.addDirectCluster(directCluster2), true);
    QCOMPARE(sink.addDirectCluster(directCluster3), true);
    //-------------------------------------------------------------
    //obtain sink paths for mapped clusters
    forwardCluster1->sendSinkPathReq();
    forwardCluster2->sendSinkPathReq();
    forwardCluster1->sendSinkPathReq();
    //forwardowanie nie dziala - poprawic
    forwardCluster3->sendSinkPathReq();
    forwardCluster4->sendSinkPathReq();
    forwardCluster5->sendSinkPathReq();
    //check if clusters are mapped by sink
    QCOMPARE(sink.checkIfHasDirectCluster(directCluster1), true);
    QCOMPARE(sink.checkIfHasDirectCluster(directCluster2), true);
    QCOMPARE(sink.checkIfHasDirectCluster(directCluster3), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster1->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster2->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster3->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster4->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster5->getNodeID(), layer.getLayerId()), true);
    //clusters in correct states
    QVERIFY(forwardCluster1->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster2->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster3->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster4->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster5->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(directCluster1->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    QVERIFY(directCluster2->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    QVERIFY(directCluster3->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    //check if mapped clusters have correct paths
    QVector<quint16> sinkPath1{1};
    QVector<quint16> sinkPath2{1};
    QVector<quint16> sinkPath3{4, 1};
    QVector<quint16> sinkPath4{3};
    QVector<quint16> sinkPath5{2};
    QVERIFY(forwardCluster1->getSinkPath() == sinkPath1);
    QVERIFY(forwardCluster2->getSinkPath() == sinkPath2);
    QVERIFY(forwardCluster3->getSinkPath() == sinkPath3);
    QVERIFY(forwardCluster4->getSinkPath() == sinkPath4);
    QVERIFY(forwardCluster5->getSinkPath() == sinkPath5);
    //register for sink broadcasting signal
    QSignalSpy sinkBroadcastSignal(&sink, SIGNAL(broadCastDataToClusters(DataFrame)));
    QCOMPARE(sinkBroadcastSignal.count(), 0);
    //remove a mapped cluster node from layer
    QCOMPARE(layer.removeNode(forwardCluster2->getNodeID()), true);
    QCOMPARE(sinkBroadcastSignal.count(), 1);
    QCOMPARE(sink.checkIfHasMappedCluster(5, layer.getLayerId()), false);
    //check if path updated after node removed
    sinkPath3 = QVector<quint16>{4, 1};
    QVERIFY(forwardCluster3->getSinkPath() == sinkPath3);
    //check if direct node removed
    QCOMPARE(layer.removeNode(directCluster3->getNodeID()), true);
    QCOMPARE(sinkBroadcastSignal.count(), 2);
    QCOMPARE(sink.checkIfHasMappedCluster(3, layer.getLayerId()), false);
    //check if path updated after node removed
    sinkPath5 = QVector<quint16>{2};
    QVERIFY(forwardCluster5->getSinkPath() == sinkPath5);
    //remove all direct clusters - no messages to be exchanged in the network
    QCOMPARE(layer.removeNode(directCluster1->getNodeID()), true);
    QCOMPARE(sinkBroadcastSignal.count(), 3);
    QCOMPARE(sink.checkIfHasMappedCluster(1, layer.getLayerId()), false);
    sinkPath1 = QVector<quint16>{6, 2};
    QVERIFY(forwardCluster1->getSinkPath() == sinkPath1);
    QCOMPARE(layer.removeNode(directCluster2->getNodeID()), true);
    QCOMPARE(sinkBroadcastSignal.count(), 4); // sent last signal telling network that no direct clusters are available
    QCOMPARE(sink.checkIfHasMappedCluster(2, layer.getLayerId()), false);
    //check if sink has no clusters connected directly
    QCOMPARE(sink.getNumOfInRangeCusters() > 0, false);
    //check network nodes states
    QVERIFY(forwardCluster1->getSinkPath().isEmpty());
    QVERIFY(forwardCluster1->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(forwardCluster3->getSinkPath().isEmpty());
    QVERIFY(forwardCluster3->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(forwardCluster4->getSinkPath().isEmpty());
    QVERIFY(forwardCluster4->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(forwardCluster5->getSinkPath().isEmpty());
    QVERIFY(forwardCluster5->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    //create new cluster to replace the old one in forwarding the data to sink
    ClusterNode *directCluster4 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 9));
    directCluster4->setNodePosition(QPoint(12, 10));
    directCluster4->setNodeRange(15);

    //Connecting cluster nodes-------------------------------------
    //connect mapped clusters
    //C1
    QCOMPARE(forwardCluster1->connectToNode(directCluster4), true);
    //C3
    QCOMPARE(forwardCluster3->connectToNode(directCluster4), true);
    //C4
    QCOMPARE(forwardCluster4->connectToNode(directCluster4), true);
    //C5
    QCOMPARE(forwardCluster5->connectToNode(directCluster4), true);

    //connect direct clusters to sink
    QCOMPARE(sink.addDirectCluster(directCluster4), true);
    //-------------------------------------------------------------
    //obtain sink paths for mapped clusters
    forwardCluster1->sendSinkPathReq();
    forwardCluster3->sendSinkPathReq();
    forwardCluster4->sendSinkPathReq();
    forwardCluster5->sendSinkPathReq();
    //check if clusters are mapped by sink
    QCOMPARE(sink.checkIfHasDirectCluster(directCluster4), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster1->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster3->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster4->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster5->getNodeID(), layer.getLayerId()), true);
    //clusters in correct states
    QVERIFY(forwardCluster1->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster3->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster4->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster5->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(directCluster4->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    //check if mapped clusters have correct paths
    QVector<quint16> sinkPath6{directCluster4->getNodeID()};
    QVERIFY(forwardCluster1->getSinkPath() == sinkPath6);
    QVERIFY(forwardCluster3->getSinkPath() == sinkPath6);
    QVERIFY(forwardCluster4->getSinkPath() == sinkPath6);
    QVERIFY(forwardCluster5->getSinkPath() == sinkPath6);
}
//TDO
void Test_SinkNode::test_disconnectSinkFromNetwork()
{
    //set all connected network states to disconnected and disable communication until
    // new sink added in range of at least one cluster
    SinkNode sink;
    sink.setPosition(QPoint(10, 10));
    sink.setRange(4);
    qRegisterMetaType<DataFrame>();

    NetworkLayer layer(0);
    ClusterNode *directCluster1 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 1));
    ClusterNode *directCluster2 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 2));
    ClusterNode *directCluster3 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 3));
    ClusterNode *forwardCluster1 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 4));
    ClusterNode *forwardCluster2 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 5));
    ClusterNode *forwardCluster3 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 6));
    ClusterNode *forwardCluster4 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 7));
    ClusterNode *forwardCluster5 = static_cast<ClusterNode*>(layer.createNode(NetworkNode::NodeType::Cluster, 8));

    //set node parameters
    //direct clusters
    directCluster1->setNodePosition(QPoint(10, 7));
    directCluster1->setNodeRange(15);
    directCluster2->setNodePosition(QPoint(10, 14));
    directCluster2->setNodeRange(15);
    directCluster3->setNodePosition(QPoint(12, 10));
    directCluster3->setNodeRange(15);
    //mapped clusters
    forwardCluster1->setNodePosition(QPoint(11, 7));
    forwardCluster1->setNodeRange(15);
    forwardCluster2->setNodePosition(QPoint(10, 5));
    forwardCluster2->setNodeRange(15);
    forwardCluster3->setNodePosition(QPoint(11, 5));
    forwardCluster3->setNodeRange(15);
    forwardCluster4->setNodePosition(QPoint(16, 10));
    forwardCluster4->setNodeRange(15);
    forwardCluster5->setNodePosition(QPoint(12, 15));
    forwardCluster5->setNodeRange(15);
    //Connecting cluster nodes-------------------------------------
    //connect mapped clusters
    //C1
    QCOMPARE(forwardCluster1->connectToNode(forwardCluster2), true);
    QCOMPARE(forwardCluster1->connectToNode(forwardCluster3), true);
    QCOMPARE(forwardCluster1->connectToNode(directCluster1), true);
    //C2
    QCOMPARE(forwardCluster2->connectToNode(forwardCluster3), true);
    QCOMPARE(forwardCluster2->connectToNode(directCluster1), true);
    QCOMPARE(forwardCluster2->connectToNode(directCluster2), true);
    //C3
    QCOMPARE(forwardCluster3->connectToNode(directCluster2), true);
    //C4
    QCOMPARE(forwardCluster4->connectToNode(directCluster1), true);
    QCOMPARE(forwardCluster4->connectToNode(directCluster3), true);
    //C5
    QCOMPARE(forwardCluster5->connectToNode(directCluster2), true);
    QCOMPARE(forwardCluster5->connectToNode(directCluster3), true);
    //connect direct clusters to sink
    QCOMPARE(sink.addDirectCluster(directCluster1), true);
    QCOMPARE(sink.addDirectCluster(directCluster2), true);
    QCOMPARE(sink.addDirectCluster(directCluster3), true);
    //-------------------------------------------------------------
    //obtain sink paths for mapped clusters
    forwardCluster1->sendSinkPathReq();
    forwardCluster2->sendSinkPathReq();
    forwardCluster1->sendSinkPathReq();
    //forwardowanie nie dziala - poprawic
    forwardCluster3->sendSinkPathReq();
    forwardCluster4->sendSinkPathReq();
    forwardCluster5->sendSinkPathReq();
    //check if clusters are mapped by sink
    QCOMPARE(sink.checkIfHasDirectCluster(directCluster1), true);
    QCOMPARE(sink.checkIfHasDirectCluster(directCluster2), true);
    QCOMPARE(sink.checkIfHasDirectCluster(directCluster3), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster1->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster2->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster3->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster4->getNodeID(), layer.getLayerId()), true);
    QCOMPARE(sink.checkIfHasMappedCluster(forwardCluster5->getNodeID(), layer.getLayerId()), true);
    //clusters in correct states
    QVERIFY(forwardCluster1->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster2->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster3->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster4->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(forwardCluster5->getCurrentState() == ClusterNode::ClusterStates::CONNECTED);
    QVERIFY(directCluster1->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    QVERIFY(directCluster2->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    QVERIFY(directCluster3->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK);
    //sink paths defined
    QVERIFY(!forwardCluster1->getSinkPath().isEmpty());
    QVERIFY(!forwardCluster2->getSinkPath().isEmpty());
    QVERIFY(!forwardCluster3->getSinkPath().isEmpty());
    QVERIFY(!forwardCluster4->getSinkPath().isEmpty());
    QVERIFY(!forwardCluster5->getSinkPath().isEmpty());
    //disconnect from all nodes
    sink.disconnectSinkFromNetwork();
    //all states changed
    QVERIFY(forwardCluster1->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(forwardCluster2->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(forwardCluster3->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(forwardCluster4->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(forwardCluster5->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(directCluster1->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(directCluster2->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    QVERIFY(directCluster3->getCurrentState() == ClusterNode::ClusterStates::DISCONNECTED);
    //sink paths reset
    QVERIFY(forwardCluster1->getSinkPath().isEmpty());
    QVERIFY(forwardCluster2->getSinkPath().isEmpty());
    QVERIFY(forwardCluster3->getSinkPath().isEmpty());
    QVERIFY(forwardCluster4->getSinkPath().isEmpty());
    QVERIFY(forwardCluster5->getSinkPath().isEmpty());
    QVERIFY(directCluster1->getSinkPath().isEmpty());
    QVERIFY(directCluster2->getSinkPath().isEmpty());
    QVERIFY(directCluster3->getSinkPath().isEmpty());
}
