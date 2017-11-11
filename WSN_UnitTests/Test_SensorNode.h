#ifndef TEST_SENSORNODE_H
#define TEST_SENSORNODE_H
#include <QObject>
#include <QtTest/QtTest>

class Test_SensorNode : public QObject
{
private slots:
    void test_connectToCluster();
    void test_disconnectFromCluster();
    void test_processData();

};

#endif // TEST_SENSORNODE_H
