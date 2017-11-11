#include "test_NetworkLayer.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"

void Test_NetworkLayer::test_createNode()
{
    NetworkLayer layer(0);
    quint16 sensor_id = 0, cluster_id = 1;
    QCOMPARE(layer.getNumOfNodes(), static_cast<quint16>(0));
    QCOMPARE(layer.createNode(NetworkNode::NodeType::Sensor, sensor_id), true);
    QCOMPARE(layer.getNumOfNodes(), static_cast<quint16>(1));
    QCOMPARE(layer.createNode(NetworkNode::NodeType::Cluster, cluster_id), true);
    QCOMPARE(layer.getNumOfNodes(), static_cast<quint16>(2));
    QCOMPARE(layer.createNode(NetworkNode::NodeType::Cluster, cluster_id), false);
    QCOMPARE(layer.createNode(NetworkNode::NodeType::Sensor, cluster_id), false);
    QCOMPARE(layer.getNumOfNodes(), static_cast<quint16>(2));
    QCOMPARE(layer.createNode(NetworkNode::NodeType::NoType, 10), false);
}

void Test_NetworkLayer::test_connectNodes()
{
    NetworkLayer layer(0);
    quint16 sensor1_id = 0, cluster1_id = 1, cluster2_id = 2, sensor2_id = 3;
    layer.createNode(NetworkNode::NodeType::Sensor, sensor1_id);
    layer.createNode(NetworkNode::NodeType::Sensor, sensor2_id);
    layer.createNode(NetworkNode::NodeType::Cluster, cluster1_id);
    layer.createNode(NetworkNode::NodeType::Cluster, cluster2_id);
    QCOMPARE(layer.getNumOfNodes(), static_cast<quint16>(4));
    QCOMPARE(layer.connectNodes(sensor1_id, cluster1_id), true);
    QCOMPARE(layer.connectNodes(sensor2_id, cluster1_id), true);

}

void Test_NetworkLayer::test_connectNodeWidget()
{

}

void Test_NetworkLayer::test_removeNode()
{

}

void Test_NetworkLayer::test_setLayerId()
{

}

void Test_NetworkLayer::test_getLayerId()
{

}
