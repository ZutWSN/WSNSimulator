#include "SensorNetwork.h"
#include <utility>

SensorNetwork::SensorNetwork()
{

}

SensorNetwork::~SensorNetwork()
{
    for (NetworkLayer *layer : m_layers)
    {
        delete layer;
    }
}

SensorNetwork::SensorNetwork(const SensorNetwork &other)
{
    if(this != &other)
    {
        if(!m_layers.empty())
        {
            for(NetworkLayer *layer : m_layers)
            {
                delete layer;
            }
            m_layers.clear();
        }
        for(NetworkLayer *layer : other.m_layers)
        {
            NetworkLayer *new_layer = new NetworkLayer(*layer);
            if(new_layer != nullptr)
            {
                m_layers.push_back(new_layer);
            }
        }
    }
}

SensorNetwork::SensorNetwork(SensorNetwork &&other)
{
    if(this != &other)
    {
        if(!m_layers.empty())
        {
            for(NetworkLayer *layer : m_layers)
            {
                delete layer;
            }
            m_layers.clear();
        }
        for(NetworkLayer *layer : other.m_layers)
        {
            NetworkLayer *new_layer = layer;
            if(new_layer != nullptr)
            {
                m_layers.push_back(new_layer);
                layer = nullptr;
            }
        }
    }
}

SensorNetwork& SensorNetwork::operator=(const SensorNetwork &other)
{
    if(this != &other)
    {
        if(!m_layers.empty())
        {
            for(NetworkLayer *layer : m_layers)
            {
                delete layer;
            }
            m_layers.clear();
        }
        for(NetworkLayer *layer : other.m_layers)
        {
            NetworkLayer *new_layer = new NetworkLayer(*layer);
            if(new_layer != nullptr)
            {
                m_layers.push_back(new_layer);
            }
        }
    }
    return *this;
}

SensorNetwork& SensorNetwork::operator=(SensorNetwork &&other)
{
    if(this != &other)
    {
        if(!m_layers.empty())
        {
            for(NetworkLayer *layer : m_layers)
            {
                delete layer;
            }
            m_layers.clear();
        }
        for(NetworkLayer *layer : other.m_layers)
        {
            NetworkLayer *new_layer = layer;
            if(new_layer != nullptr)
            {
                m_layers.push_back(new_layer);
                layer = nullptr;
            }
        }
    }
    return *this;
}

bool SensorNetwork::addLayer(qint16 layer_id)
{
    bool hasLayer = false;
    if(!checkIfHasLayer(layer_id))
    {
        m_layers.push_back(new NetworkLayer(layer_id));
        hasLayer = true;
    }
    return hasLayer;
}



quint16 SensorNetwork::getNumberOfLayers() const
{
    return static_cast<quint16>(m_layers.size());
}

bool SensorNetwork::checkIfHasLayer(qint16 layer_id) const
{
    bool hasLayer = false;
    for(auto &layer : m_layers)
    {
        if(layer->getLayerId() == layer_id)
        {
            hasLayer = true;
            break;
        }
    }
    return hasLayer;
}

NetworkLayer* SensorNetwork::getLayer(qint16 layer_id) const
{
    NetworkLayer* layerPtr = nullptr;
    for(NetworkLayer *layer : m_layers)
    {
        if(layer->getLayerId() == layer_id)
        {
            layerPtr = layer;
        }
    }
    return layerPtr;
}


