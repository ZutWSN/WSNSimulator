#include "Test_NetworkNodes.h"
#include <QByteArray>
#include <QDebug>
#include <QSignalSpy>
#include <widgetfactory.h>
#include "NetworkLayer.h"
#include "NetworkNode.h"
#include <QScopedPointer>

void Test_NetworkNodes::test_sendData()
{
    NetworkNode sensor;
    //for signals and slots - has to register non Qt object type
    qRegisterMetaType<DataFrame>();
    //register for signals and store count
    QSignalSpy spy(&sensor, SIGNAL(dataSend(const DataFrame&)));
    //set message
    DataFrame frame1;
    frame1.setMsg(QByteArray("Here's Johnny!"));
    frame1.setMsgType(DataFrame::RxData::NEW_DATA);
    //test if data send
    QCOMPARE(spy.count(), 0);
    QCOMPARE(sensor.sendData(frame1), true);
    QCOMPARE(spy.count(), 1);
}

void Test_NetworkNodes::test_connectToNode()
{
    NetworkLayer layer(0);
    qRegisterMetaType<DataFrame>();
    NetworkNode *sender = layer.createNode(NetworkNode::NodeType::Sensor, 0);
    NetworkNode *receiver = layer.createNode(NetworkNode::NodeType::Cluster, 1);
    //test nodes on the same layer
    QCOMPARE(sender->connectToNode(receiver), true);
    //test nodes same id error
    receiver->setNodeID(0);
    QCOMPARE(sender->connectToNode(receiver), false);
    //test nodes on different layers
    NetworkLayer layer2(1);
    NetworkNode *receiver1 = layer2.createNode(NetworkNode::NodeType::Cluster, 1);
    QCOMPARE(sender->connectToNode(receiver1), false);

}

void Test_NetworkNodes::test_connectToNodeWidget()
{
    NetworkNode node;
    WidgetFactory wFactory;
    QScopedPointer<DragWidget> widgetPtr(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor));
    QCOMPARE(node.connectToNodeWidget(widgetPtr.data()), true);
    QCOMPARE(node.connectToNodeWidget(nullptr), false);
}

void Test_NetworkNodes::test_onReceivedData()
{
    NetworkLayer layer(0);
    qRegisterMetaType<DataFrame>();
    //test if not added to layer
    NetworkNode *sender = layer.createNode(NetworkNode::NodeType::Sensor, 0);
    QVERIFY(sender != nullptr);
    NetworkNode *receiver = layer.createNode(NetworkNode::NodeType::Cluster, 0);
    QVERIFY(receiver == nullptr);
    QSignalSpy senderSendMonitor(sender, SIGNAL(dataSend(const DataFrame&)));


    QCOMPARE(senderSendMonitor.count(), 0);
    //test receiver not present on layer
    QCOMPARE(sender->connectToNode(receiver), false);

    //test case NEW DATA
    receiver = layer.createNode(NetworkNode::NodeType::Cluster, 1);
    QVERIFY(receiver != nullptr);
    QCOMPARE(sender->connectToNode(receiver), true);
    QSignalSpy receivedNewDataMonitor(receiver, SIGNAL(receivedNewData(const DataFrame&)));
    DataFrame frame1;
    frame1.setMsg(QByteArray("I'am your father"));
    frame1.setMsgType(DataFrame::RxData::NEW_DATA);
    QCOMPARE(receivedNewDataMonitor.count(), 0);
    sender->sendData(frame1);
    QCOMPARE(senderSendMonitor.count(), 1);
    QCOMPARE(receivedNewDataMonitor.count(), 1);

    //test case RECEIVED DATA - receiver sends back to sender that it received and processed data
    QSignalSpy receiverSendMonitor(receiver, SIGNAL(dataSend(const DataFrame&)));
    DataFrame frame2;
    frame2.setMsg(QByteArray("No it's impossible!"));
    frame2.setMsgType(DataFrame::RxData::RECEIVED_DATA);
    QCOMPARE(receiverSendMonitor.count(), 0);
    QCOMPARE(sender->getSendDataReceived(), false);
    receiver->sendData(frame2);
    QCOMPARE(receiverSendMonitor.count(), 1);
    QCOMPARE(receivedNewDataMonitor.count(), 1);
    QCOMPARE(sender->getSendDataReceived(), true);
}
