#ifndef SENSORNETWORK_H
#define SENSORNETWORK_H
#include "NetworkLayer.h"
#include "sinknode.h"

class SensorNetwork : public QObject
{
    Q_OBJECT
public:
    SensorNetwork();
    ~SensorNetwork();
    SensorNetwork(const SensorNetwork &other);
    SensorNetwork(SensorNetwork &&other);
    SensorNetwork& operator=(const SensorNetwork &other);
    SensorNetwork& operator=(SensorNetwork &&other);

    bool addLayer(qint16 layer_id);

    quint16 getNumberOfLayers() const;
    NetworkLayer* getLayer(qint16 layer_id) const;
    bool networkHasSink() const;
public slots:
    void onSinkAdded(const QPoint &position, quint16 range, QWidget *uiWidget);
    void onNewClusterAdded(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
    void onNewSensorAdded(quint16 sensor_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
    void onSinkRemoved();
    void onClusterRemoved(quint16 cluster_id, quint16 layer_id);
    void onSensorRemoved(quint16 sensor_id, quint16 layer_id);

private:
    bool checkIfHasLayer(qint16 layer_id) const;
    bool connectSensorToNearestCluster();
private:
    QVector<NetworkLayer*> m_layers;
    SinkNode *m_sink;
};

#endif // SENSORNETWORK_H
