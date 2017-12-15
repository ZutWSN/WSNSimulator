#include "test_NetworkLayer.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"
#include "widgetfactory.h"

void Test_NetworkLayer::test_createNode()
{
    NetworkLayer layer(0);
    quint16 sensor_id = 0, cluster_id = 1;
    QCOMPARE(layer.getNumOfNodes(), static_cast<quint16>(0));
    QVERIFY(layer.createNode(NetworkNode::NodeType::Sensor, sensor_id) !=  nullptr);
    QCOMPARE(layer.getNumOfNodes(), static_cast<quint16>(1));
    QVERIFY(layer.createNode(NetworkNode::NodeType::Cluster, cluster_id) != nullptr);
    QCOMPARE(layer.getNumOfNodes(), static_cast<quint16>(2));
    QVERIFY(layer.createNode(NetworkNode::NodeType::Cluster, cluster_id) == nullptr);
    QVERIFY(layer.createNode(NetworkNode::NodeType::Sensor, cluster_id) == nullptr);
    QCOMPARE(layer.getNumOfNodes(), static_cast<quint16>(2));
    QVERIFY(layer.createNode(NetworkNode::NodeType::NoType, 10) == nullptr);
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
    NetworkLayer layer(0);
    layer.createNode(NetworkNode::NodeType::Sensor, 0);
    WidgetFactory wFactory;
    QScopedPointer<DragWidget> sensorWgt(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor));
    //connect to new widget
    QCOMPARE(layer.connectNodeWidget(0, static_cast<QWidget*>(sensorWgt.data())), true);
    //connect to widget error, layer does not have that node id
    QCOMPARE(layer.connectNodeWidget(1, static_cast<QWidget*>(sensorWgt.data())), false);
    //connect to widget with another node before disconnecting previously connected node - false
    //fix in widgets branch
    layer.createNode(NetworkNode::NodeType::Sensor, 1);
    QCOMPARE(layer.connectNodeWidget(1, static_cast<QWidget*>(sensorWgt.data())), false);
    //connect widget after other node disconnects from it - change node test and code
    layer.disconnectNodeWidget(0);
    QCOMPARE(layer.connectNodeWidget(1, static_cast<QWidget*>(sensorWgt.data())), true);
}

void Test_NetworkLayer::test_removeNode()
{
    NetworkLayer layer(0);
    QCOMPARE(layer.removeNode(0), false);
    layer.createNode(NetworkNode::NodeType::Cluster, 0);
    QCOMPARE(layer.removeNode(0), true);
}

void Test_NetworkLayer::test_setLayerId()
{
    NetworkLayer layer(0);
    QVERIFY(layer.getLayerId() == 0);
    layer.setLayerId(10);
    QVERIFY(layer.getLayerId() == 10);
}
