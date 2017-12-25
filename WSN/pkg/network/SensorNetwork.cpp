#include "SensorNetwork.h"
#include "SensorWindow.h"
#include "ClusterNode.h"
#include "SensorNode.h"
#include <utility>

SensorNetwork::SensorNetwork():
    m_sink(nullptr)
{

}

SensorNetwork::~SensorNetwork()
{
    for (NetworkLayer *layer : m_layers)
    {
        if(layer)
        {
            delete layer;
        }
    }
    if(m_sink)
    {
        delete m_sink;
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

void SensorNetwork::onSinkAdded(const QPoint &position, quint16 range, QWidget *uiWidget)
{
    if(!networkHasSink())
    {
        //don't add new one or move this one, if sink already added
        m_sink = new SinkNode(position, range);
    }
}

void SensorNetwork::onNewClusterAdded(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget)
{
    NetworkLayer *layer = getLayer(layer_id);
    if(!layer)
    {
        layer = new NetworkLayer(layer_id);
        m_layers.push_back(layer);
    }
    layer->createNode(NetworkNode::NodeType::Cluster, cluster_id);
    ClusterNode *newCluster = static_cast<ClusterNode*>(layer->getNode(cluster_id));
    if(newCluster)
    {
        newCluster->setNodePosition(position);
        newCluster->setNodeRange(range);
        //connect to coresponding widget
        newCluster->connectToNodeWidget(uiWidget);
        bool directClusterAdded = false;
        //check if has sink and can connect to it
        if(m_sink)
        {
            double sinkDistance = newCluster->getDistanceFromNode(m_sink->getSinkPosition());
            if(range >= sinkDistance && m_sink->getSinkRange() >= sinkDistance)
            {
                directClusterAdded = m_sink->addDirectCluster(newCluster);
            }
        }
        if(layer->getNumOfNodes() > 0)
        {
            //go through all cluster nodes and connect to the nearest one
            quint16 i = 0;
            for(QVector<NetworkNode*>::const_iterator node = layer->getIteratorToFirstNode(); i < layer->getNumOfNodes(); node++)
            {
                ++i;
                if((*node)->getNodeID() != cluster_id)
                {
                    if((*node)->getNodeType() == NetworkNode::NodeType::Cluster)
                    {
                        double distance = (*node)->getDistanceFromNode(position);
                        if(distance <= range)
                        {
                            newCluster->connectToNode(*node);
                        }
                    }
                }
            }
            if(!directClusterAdded)
            {
                newCluster->sendSinkPathReq();
            }
            else
            {
                m_sink->sendNewPaths(layer_id);
            }
        }
    }

        //---------------------------------------------------------------------
        //Later reasingning sensors so that if
        //they are closer to this custer than previous ones connect them to this
        //this should be done by sending message to other cluster and sending
        //which will reasign those sensors by sending them the message of
        //new cluster position and id
}

void SensorNetwork::onNewSensorAdded(quint16 sensor_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget)
{

}

void SensorNetwork::onSinkRemoved()
{
    if(networkHasSink())
    {
        m_sink->disconnectFromWidget();
        m_sink->disconnectSinkFromNetwork();
        delete m_sink;
    }
}

void SensorNetwork::onClusterRemoved(quint16 cluster_id, quint16 layer_id)
{

}

void SensorNetwork::onSensorRemoved(quint16 sensor_id, quint16 layer_id)
{

}

bool SensorNetwork::networkHasSink() const
{
    return m_sink != nullptr;
}


