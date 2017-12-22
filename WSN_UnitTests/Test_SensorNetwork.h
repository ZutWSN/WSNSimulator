#ifndef TEST_SENSORNETWORK_H
#define TEST_SENSORNETWORK_H
#include <QObject>
#include <QtTest/QtTest>

class Test_SensorNetwork : public QObject
{
    Q_OBJECT
private slots:
    void test_addLayer();
    void test_getLayer();
    void test_checkIfHasLayer();
    void test_onNewClusterAdded();
    void test_onNewSensorAdded();
};

#endif // TEST_SENSORNETWORK_H
