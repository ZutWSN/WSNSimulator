#include "Test_NetworkNodes.h"
#include <QByteArray>
#include <QDebug>
#include <QSignalSpy>
#include <widgetfactory.h>
#include "NetworkLayer.h"
#include "NetworkNode.h"


void Test_NetworkNodes::initTestCase()
{
    qDebug("\nStart Network Unit Tests!\n");
}

void Test_NetworkNodes::test_addToLayer()
{
    NetworkNode sender(0);
    NetworkNode receiver(1);
    NetworkLayer layer(0);
    layer.addNode(&sender);
    layer.addNode(&receiver);
    QCOMPARE(sender.getNodeLayer(), layer.getLayerId());
    QCOMPARE(receiver.getNodeLayer(), layer.getLayerId());
}

void Test_NetworkNodes::test_removeFromLayer()
{
    NetworkNode sender(0);
    NetworkNode receiver(1);
    NetworkLayer layer(0);
    layer.addNode(&sender);
    layer.addNode(&receiver);
    layer.removeNode(&sender);
    layer.removeNode(&receiver);
    QCOMPARE(sender.getNodeLayer() == layer.getLayerId(), false);
    QCOMPARE(receiver.getNodeLayer() == layer.getLayerId(), false);
}

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
    NetworkLayer layer(0);
    layer.addNode(&sender);
    layer.addNode(&receiver);
    //test nodes on the same layer
    QCOMPARE(sender.connectToNode(&receiver), true);
    //test nodes same id error
    receiver.setNodeID(0);
    QCOMPARE(sender.connectToNode(&receiver), false);
    //test nodes on different layers
    NetworkLayer layer2(1);
    receiver.setNodeID(1);
    layer.removeNode(&receiver);
    layer2.addNode(&receiver);
    QCOMPARE(sender.connectToNode(&receiver), false);

}

void Test_NetworkNodes::test_onReceivedData()
{
    NetworkNode sender(0);
    NetworkNode receiver(0);
    NetworkLayer layer(0);
    qRegisterMetaType<DataFrame>();
    QSignalSpy dataSendMonitor(&sender, SIGNAL(dataSend(const DataFrame&)));
    layer.addNode(&sender);
    layer.addNode(&receiver);
    //test receiver not present on layer
    QCOMPARE(sender.connectToNode(&receiver), false);
    QCOMPARE(dataSendMonitor.count(), 0);
    //test case RECEIVED DATA
    receiver.setNodeID(1);
    layer.addNode(&receiver);
    QCOMPARE(sender.connectToNode(&receiver), true);
    DataFrame frame1;
    frame1.setMsg(QByteArray("Here's Johnny!"));
    frame1.setMsgType(DataFrame::RxData::RECEIVED_DATA);
    sender.sendData(frame1);
    QCOMPARE(dataSendMonitor.count(), 1);
    QCOMPARE(receiver.getSendDataReceived(), true);
    //test case NEW DATA
    QSignalSpy receivedNewDataMonitor(&receiver, SIGNAL(receivedNewData(DataFrame)));
    DataFrame frame2;
    frame2.setMsg(QByteArray("I'am your father"));
    frame2.setMsgType(DataFrame::RxData::NEW_DATA);
    QCOMPARE(receivedNewDataMonitor.count(), 0);
    sender.sendData(frame2);
    QCOMPARE(dataSendMonitor.count(), 2);
    QCOMPARE(receivedNewDataMonitor.count(), 1);
}



void Test_NetworkNodes::test_WidgetFactory()
{
    WidgetFactory wFactory;

    DragWidget *sensor = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor);
    DragWidget *cluster = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster);
    DragWidget *sink = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sink);

    bool sensorCreated = (sensor->getWidgetType() == DragWidget::DragWidgetType::Sensor);
    bool clusterCreated = (cluster->getWidgetType() == DragWidget::DragWidgetType::Cluster);
    bool sinkCreated = (sink->getWidgetType() == DragWidget::DragWidgetType::Sink);

    qDebug("\nTest widgets creation if returns proper widget type\n");

    QVERIFY(sensorCreated == true);
    QVERIFY(clusterCreated == true);
    QVERIFY(sinkCreated == true);

    qDebug("\nTest returning wrong widget\n");
    DragWidget *widget = wFactory.getNewDragWidget(static_cast<DragWidget::DragWidgetType>(10));
    QVERIFY(widget == nullptr);

}

void Test_NetworkNodes::cleanupTestCase()
{
    qDebug("\nNetwork Unit Tests Finished!\n");
}

