#include "test_sinknode.h"
#include "sinknode.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"

void Test_SinkNode::test_getSinkInstance()
{
    SinkNode *sink = SinkNode::getSinkInstance(QPoint(0, 0), 20);
    QVERIFY(sink != nullptr);
    QCOMPARE(SinkNode::getSinkInstance(QPoint(0, 0), 20), sink);
}

void Test_SinkNode::test_addRemoveDirectCluster()
{
    SinkNode *sink = SinkNode::getSinkInstance(QPoint(0, 0), 20);
    NetworkLayer layer(0);
    NetworkNode *cluster1 = layer.createNode(NetworkNode::NodeType::Cluster, 0);
    NetworkNode *cluster2 = layer.createNode(NetworkNode::NodeType::Cluster, 1);
    cluster1->setNodePosition(QPoint(0, 10));
    cluster2->setNodePosition(QPoint(0, 21));
    QCOMPARE(sink->addDirectCluster(cluster1), true);
    QCOMPARE(sink->addDirectCluster(cluster2), false);
    QCOMPARE(sink->removeDirectCluster(cluster1), true);
    QCOMPARE(sink->removeDirectCluster(cluster2), false);
}

void Test_SinkNode::test_onReceivedDataFromCluster()
{

}
