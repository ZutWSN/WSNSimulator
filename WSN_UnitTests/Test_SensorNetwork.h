#ifndef TEST_SENSORNETWORK_H
#define TEST_SENSORNETWORK_H
#include <QObject>
#include <QtTest/QtTest>

class Test_SensorNetwork : public QObject
{
    Q_OBJECT
private slots:
    void test_onSinkAdded();
    void test_onNewClusterAdded();
    void test_onNewSensorAdded();
    void test_onSinkRemoved();
    void test_onNewClusterRemoved();
    void test_onNewSensorRemoved();
signals:
    //widget sent slot calls
    void addNewSink(const QPoint &postion, quint16 range);
    void addNewCluster();
    void addNewSensor();
};

#endif // TEST_SENSORNETWORK_H
