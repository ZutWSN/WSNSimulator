#include "Test_NetworkNodes.h"
#include "widgetfactory.h"
#include "NetworkNode.h"
#include "SensorNode.h"
#include "ClusterNode.h"
#include "SinkWidget.h"
#include "SensorWidget.h"
#include "ClusterWidget.h"
#include "SinkNode.h"
#include <QByteArray>
#include <QDebug>
#include <QSignalSpy>

void Test_NetworkNodes::initTestCase()
{
    qDebug("\nStart Network Unit Tests!\n");
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

}

void Test_NetworkNodes::test_WidgetFactory()
{
    WidgetFactory wFactory;

    DragWidget *sensor = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor);
    DragWidget *cluster = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster);
    DragWidget *sink = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sink);

    bool sensorCreated = (static_cast<SensorWidget*>(sensor) != NULL);
    bool clusterCreated = (static_cast<SensorWidget*>(cluster) != NULL);
    bool sinkCreated = (static_cast<SensorWidget*>(sink) != NULL);

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

