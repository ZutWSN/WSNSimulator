#ifndef TEST_SENSORNETWORK_H
#define TEST_SENSORNETWORK_H
#include <QObject>
#include <QtTest/QtTest>

class Test_SensorNetwork : public QObject
{
    Q_OBJECT
public:
    void test_addLayer();
    void test_createNode();
    void test_connectNodes();
    void test_findBestPath();
};

#endif // TEST_SENSORNETWORK_H
