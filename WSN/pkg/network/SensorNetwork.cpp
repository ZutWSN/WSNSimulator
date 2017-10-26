#include "SensorNetwork.h"

SensorNetwork::SensorNetwork():
    m_layer_counter(0)
{

}

void SensorNetwork::addLayer()
{
    m_layers.push_back(new NetworkLayer(static_cast<qint16>(m_layer_counter)));
    ++m_layer_counter;
}

quint16 SensorNetwork::getNumberOfLayers() const
{
    return m_layer_counter;
}

