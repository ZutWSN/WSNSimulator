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
    NetworkNode sender(0);
    NetworkNode receiver(1);
    sender.setLayer(0);
    receiver.setLayer(0);
    //test nodes on the same layer
    QCOMPARE(sender.connectToNode(&receiver), true);
    QCOMPARE(sender.checkIfConnectedToNode(&receiver), true);
    //test nodes same id error
    NetworkNode receiver1(0);
    receiver1.setLayer(0);
    QCOMPARE(sender.connectToNode(&receiver1), false);
    QCOMPARE(sender.checkIfConnectedToNode(&receiver1), false);
    //test nodes on different layers
    NetworkNode receiver2(2);
    receiver2.setLayer(1);
    QCOMPARE(sender.connectToNode(&receiver2), false);
    QCOMPARE(sender.checkIfConnectedToNode(&receiver2), false);

}

void Test_NetworkNodes::test_disconnectedFromNode()
{
    NetworkNode sender(0);
    NetworkNode receiver(1);
    NetworkNode receiver2(2);
    sender.setLayer(0);
    receiver.setLayer(0);
    receiver2.setLayer(1);
    //can't disconnect before connected
    QCOMPARE(sender.disconnectFromNode(&receiver), false);
    sender.connectToNode(&receiver);
    //cant disconnect , not valid node -> noet connected and different layer
    QCOMPARE(sender.disconnectFromNode(&receiver2), false);
    //disconnect
    QCOMPARE(sender.disconnectFromNode(&receiver), true);
}

void Test_NetworkNodes::test_connectToNodeWidget()
{
    NetworkNode node, node1;
    WidgetFactory wFactory;
    QScopedPointer<DragWidget> widgetPtr(wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor));
    QCOMPARE(node.connectToNodeWidget(widgetPtr.data()), true);
    QCOMPARE(node.connectToNodeWidget(nullptr), false);
    //already some node connected to that widget first disconnect it
    //fix in widget branch
    QCOMPARE(node1.connectToNodeWidget(widgetPtr.data()), false);
    //now after disconnecting it should work
    QCOMPARE(node.disconnectFromWidget(), true);
    QCOMPARE(node1.connectToNodeWidget(widgetPtr.data()), true);
}

void Test_NetworkNodes::test_onReceivedData()
{
    qRegisterMetaType<DataFrame>();
    //test if not added to layer
    NetworkNode sender(0);
    NetworkNode receiver(1);
    sender.setLayer(0);
    receiver.setLayer(0);
    QSignalSpy senderSendMonitor(&sender, SIGNAL(dataSend(const DataFrame&)));
    QCOMPARE(senderSendMonitor.count(), 0);
    //test case NEW DATA
    QCOMPARE(sender.connectToNode(&receiver), true);
    QSignalSpy receivedNewDataMonitor(&receiver, SIGNAL(receivedNewData(const DataFrame&)));
    DataFrame frame1;
    frame1.setMsg(QByteArray("I'am your father"));
    frame1.setMsgType(DataFrame::RxData::NEW_DATA);
    QCOMPARE(receivedNewDataMonitor.count(), 0);
    sender.sendData(frame1);
    QCOMPARE(senderSendMonitor.count(), 1);
    QCOMPARE(receivedNewDataMonitor.count(), 1);

    //test case RECEIVED DATA - receiver sends back to sender that it received and processed data
    QSignalSpy receiverSendMonitor(&receiver, SIGNAL(dataSend(const DataFrame&)));
    DataFrame frame2;
    frame2.setMsg(QByteArray("No it's impossible!"));
    frame2.setMsgType(DataFrame::RxData::RECEIVED_DATA);
    QCOMPARE(receiverSendMonitor.count(), 0);
    QCOMPARE(sender.getSendDataReceived(), false);
    receiver.sendData(frame2);
    QCOMPARE(receiverSendMonitor.count(), 1);
    QCOMPARE(receivedNewDataMonitor.count(), 1);
    QCOMPARE(sender.getSendDataReceived(), true);

    QVERIFY(sender.getNumOfPendingDataFrames() == 0);
    QVERIFY(receiver.getNumOfPendingDataFrames() == 0);
}
