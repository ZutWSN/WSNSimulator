#ifndef TEST_CLUSTERNODE_H
#define TEST_CLUSTERNODE_H
#include <QObject>
#include <QtTest/QtTest>
//will broadcast data to its sensor nodes, and forward and receive data to
//neighbour clusters, check if sink is in range of cluster. Only cluster
//node will be able to send data to it, check multihop wsn for information
//Cluster will send data periodically to neighbour clusters, between each send
//it will accumulate data from sensors, only when got data from all of them
//it will send data according to received packets.
//Path discovery will be done on event when new cluster is added
//to its layer, then it will receive new optimal path map from sink
//and resync with all nodes so that new cluster can synchronize with them
class Test_ClusterNode : public QObject
{
    Q_OBJECT
public slots:
    void test_broadCastDataToSensors();
    void test_onReceivedData();
    void test_processData();
};

#endif // TEST_CLUSTERNODE_H
