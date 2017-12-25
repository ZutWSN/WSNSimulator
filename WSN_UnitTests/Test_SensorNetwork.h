#ifndef TEST_SENSORNETWORK_H
#define TEST_SENSORNETWORK_H
#include <QObject>
#include <QtTest/QtTest>
#include "SensorNetwork.h"
#include "QScopedPointer"

class Test_SensorNetwork : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void test_onSinkAdded();
    void test_onNewClusterAdded();
    void test_onNewSensorAdded();
    void test_onSinkRemoved();
    void test_onNewClusterRemoved();
    void test_onNewSensorRemoved();
    void cleanupTestCase();
signals:
    //widget sent slot calls
    void addNewSink(const QPoint &position, quint16 range, QWidget *uiWidget);
    void addNewCluster(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
    void addNewSensor(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
private:
    QScopedPointer<SensorNetwork> network;
    QScopedPointer<QWidget> sinkWgt;
    QScopedPointer<QWidget> sensorWgt;
    QScopedPointer<QWidget> clusterWgt;
};

#endif // TEST_SENSORNETWORK_H
