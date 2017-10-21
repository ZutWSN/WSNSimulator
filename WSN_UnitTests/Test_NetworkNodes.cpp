#include "Test_NetworkNodes.h"
#include "NetworkNode.h"
#include "SensorNode.h"
#include "ClusterNode.h"
#include "SinkNode.h"
#include <QByteArray>
#include <QDebug>


void Test_NetworkNodes::initTestCase()
{
    qDebug("Start Network Unit Tests : \n");
}


void Test_NetworkNodes::test_sendData()
{
    NetworkNode sensor;
    QCOMPARE(sensor.sendData(QByteArray("This has Data")), false);
    QCOMPARE(sensor.sendData(QByteArray("This does not have Data")), false);
}

void Test_NetworkNodes::test_connectToNode()
{

}

void Test_NetworkNodes::cleanupTestCase()
{
    qDebug("\nNetwork Unit Tests Finish\n");
}

