#ifndef SENSORNETWORK_H
#define SENSORNETWORK_H
#include <QScopedPointer>
#include "NetworkLayer.h"
#include "NetworkNode.h"

class SensorNetwork
{
public:
    SensorNetwork();

    void addLayer();

    quint16 getNumberOfLayers() const;
private:
    quint16 m_layer_counter;
    QVector<NetworkLayer*> m_layers;
    QVector<QScopedPointer<NetworkNode> > m_nodes;
};

#endif // SENSORNETWORK_H
