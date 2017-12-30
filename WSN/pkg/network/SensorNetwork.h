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
    QVector<NetworkLayer*>::const_iterator getIteratorToFirstLayer();
    QString getSinkLastMsg() const;
    QPoint getSinkPosition() const;
    bool networkHasSink() const;
public slots:
    void onSinkAdded(const QPoint &position, quint16 range, QWidget *uiWidget);
    void onNewClusterAdded(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
    void onNewSensorAdded(quint16 sensor_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
    void onSinkRemoved();
    void onNodeRemoved(quint16 node_id, quint16 layer_id);
    void onNodeMoved(quint16 node_id, quint16 layer_id, QPoint position);
private:
    bool checkIfHasLayer(qint16 layer_id) const;
private:
    QVector<NetworkLayer*> m_layers;
    SinkNode *m_sink;
};

#endif // SENSORNETWORK_H
