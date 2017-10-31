#ifndef SENSORNETWORK_H
#define SENSORNETWORK_H
#include "NetworkLayer.h"

class SensorNetwork
{
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
private:
    bool checkIfHasLayer(qint16 layer_id) const;
private:
    QVector<NetworkLayer*> m_layers;
};

#endif // SENSORNETWORK_H
