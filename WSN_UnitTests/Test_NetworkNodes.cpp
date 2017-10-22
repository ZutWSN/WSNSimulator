#include "Test_NetworkNodes.h"
#include "NetworkNode.h"
#include "SensorNode.h"
#include "ClusterNode.h"
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

void Test_NetworkNodes::cleanupTestCase()
{
    qDebug("\nNetwork Unit Tests Finished!\n");
}

