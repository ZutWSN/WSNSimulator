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
};

#endif // TEST_SENSORNETWORK_H
