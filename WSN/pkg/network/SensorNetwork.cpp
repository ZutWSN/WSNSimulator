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

QVector<NetworkLayer*>::const_iterator SensorNetwork::getIteratorToFirstLayer()
{
    return m_layers.constBegin();
}

NetworkNode *SensorNetwork::getNetworkNode(quint16 node_id) const
{
    NetworkNode *node = nullptr;
    for(auto && layer : m_layers)
    {
        if(layer->checkIfNodeInLayer(node_id))
        {
            node = layer->getNode(node_id);
            break;
        }
    }
    return node;
}

QString SensorNetwork::getSinkLastMsg() const
{
    QString lastMsg;
    if(m_sink->getLastMsg().getSender() != m_sink->getLastMsg().getDestination())
    {
        lastMsg = m_sink->getLastMsg().getMsgInfo();
        if(!m_sink->getLastMsg().getMsg().isEmpty())
        {
            if(m_sink->getLastMsg().getMsgType() == DataFrame::RxData::NEW_DATA)
            {
                lastMsg.append("\nMESSAGE CONTENT: \n" + m_sink->getLastMsg().getMsg() + "\n");
            }
        }
    }
    return lastMsg;
}

QPoint SensorNetwork::getSinkPosition() const
{
    return m_sink->getSinkPosition();
}

void SensorNetwork::onSinkAdded(const QPoint &position, quint16 range, QWidget *uiWidget)
{
    if(!networkHasSink())
    {
        //don't add new one or move this one, if sink already added
        m_sink = new SinkNode(position, range);
        m_sink->connectToNodeWidget(uiWidget);
        //if network already has nodes try reconnecting them to new sink
        for(NetworkLayer *layer : m_layers)
        {
            if(layer->getNumOfNodes() > 0)
            {
                quint16 i = 0;
                for(QVector<NetworkNode*>::const_iterator node = layer->getIteratorToFirstNode(); i < layer->getNumOfNodes(); node++)
                {
                    ++i;
                    if((*node)->getNodeType() == NetworkNode::NodeType::Cluster)
                    {
                        double sinkDistance = (*node)->getDistanceFromNode(m_sink->getSinkPosition());
                        if(range >= sinkDistance && m_sink->getSinkRange() >= sinkDistance)
                        {
                            m_sink->addDirectCluster((*node));
                        }
                    }
                }
                m_sink->sendNewPaths(layer->getLayerId());
            }
        }
    }
}

void SensorNetwork::onNewClusterAdded(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget)
{
    if(NetworkLayer::checkIfIdAvailable(cluster_id))
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
                            if(newCluster->checkIfCanConnect(*node))
                            {
                                newCluster->connectToNode(*node);
                            }
                        }
                        else if((*node)->getNodeType() == NetworkNode::NodeType::Sensor)
                        {
                            SensorNode *sensor = static_cast<SensorNode*>(*node);
                            if(sensor->isConnectedToCluster())
                            {
                                double oldDistance = sensor->getDistanceFromNode(sensor->getClusterPosition());
                                double newDistance = sensor->getDistanceFromNode(position);
                                if(newDistance < oldDistance)
                                {
                                    NetworkNode *oldCluster = layer->getNode(sensor->getClusterID());
                                    if(oldCluster)
                                    {
                                        if(sensor->disconnectFromNode(oldCluster))
                                        {
                                            sensor->connectToNode(newCluster);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(sensor->checkIfCanConnect(newCluster))
                                {
                                    sensor->connectToNode(newCluster);
                                }
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
                    if(m_sink->getNumOfMappedClusters() > 0)
                    {
                        m_sink->sendNewPaths(layer_id);
                    }
                }
            }
        }
    }
}

void SensorNetwork::onNewSensorAdded(quint16 sensor_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget)
{
    if(NetworkLayer::checkIfIdAvailable(sensor_id))
    {
        NetworkLayer *layer = getLayer(layer_id);
        if(!layer)
        {
            layer = new NetworkLayer(layer_id);
            m_layers.push_back(layer);
        }
        double minDistance = UINT64_MAX;
        NetworkNode *closestCluster = nullptr;
        if(layer->getNumOfNodes() > 0)
        {
            //go through all cluster nodes and connect to the nearest one
            quint16 i = 0;
            for(QVector<NetworkNode*>::const_iterator node = layer->getIteratorToFirstNode(); i < layer->getNumOfNodes(); node++)
            {
                ++i;
                if((*node)->getNodeType() == NetworkNode::NodeType::Cluster)
                {
                    double distance = (*node)->getDistanceFromNode(position);
                    bool inRange = ((distance <= range) && (*node)->checkIfInRange(position));
                    if(inRange && distance < minDistance)
                    {
                        minDistance = distance;
                        closestCluster = *node;
                    }
                }
            }
        }
        layer->createNode(NetworkNode::NodeType::Sensor, sensor_id);
        NetworkNode *newSensor = layer->getNode(sensor_id);
        if(newSensor)
        {
            newSensor->setNodePosition(position);
            newSensor->setNodeRange(range);
            //connect to coresponding widget
            newSensor->connectToNodeWidget(uiWidget);
            if(closestCluster && minDistance < UINT64_MAX)
            {
                newSensor->connectToNode(closestCluster);
            }
        }
    }
}

void SensorNetwork::onSinkRemoved()
{
    if(networkHasSink())
    {
        m_sink->sendSinkRemovedBroadcast();
        m_sink->disconnectFromWidget();
        delete m_sink;
        m_sink = nullptr;
    }
}

void SensorNetwork::onNodeRemoved(quint16 node_id, quint16 layer_id)
{
    NetworkLayer *layer = getLayer(layer_id);
    if(layer)
    {
        layer->removeNode(node_id);
    }
}

void SensorNetwork::onNodeMoved(quint16 node_id, quint16 layer_id, QPoint position)
{
    NetworkLayer *layer = getLayer(layer_id);
    if(layer)
    {
        layer->moveNode(node_id, position);
        if(m_sink)
        {
            auto movedNode = layer->getNode(node_id);
            if(movedNode)
            {
                if(movedNode->getNodeType() == NetworkNode::NodeType::Cluster)
                {
                    auto cluster = static_cast<ClusterNode*>(movedNode);
                    double sinkDistance = movedNode->getDistanceFromNode(m_sink->getSinkPosition());
                    if(cluster->getNodeRange() >= sinkDistance && m_sink->getSinkRange() >= sinkDistance)
                    {
                        cluster->resetSinkPath();
                        if(m_sink->addDirectCluster(cluster))
                        {
                            if(m_sink->getNumOfMappedClusters() > 0)
                            {
                                m_sink->sendNewPaths(layer_id);
                            }
                        }
                    }
                    else
                    {
                        if(movedNode->getNumOfConnectedNodes() > 0)
                        {
                            if(cluster)
                            {
                                cluster->sendSinkPathReq();
                                m_sink->sendNewPaths(layer_id);
                            }
                        }
                    }
                }
            }
        }
    }
}

bool SensorNetwork::networkHasSink() const
{
    return m_sink != nullptr;
}


