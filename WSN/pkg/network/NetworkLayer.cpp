#include "NetworkLayer.h"
#include "NetworkNode.h"
#include "SensorNode.h"
#include "ClusterNode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <memory>

QVector<quint16> NetworkLayer::m_usedIDPool;

NetworkLayer::NetworkLayer(qint16 layer_id):
    m_layer_id(layer_id)
{

}

NetworkLayer::~NetworkLayer()
{
    for(NetworkNode *node : m_nodes)
    {
        m_usedIDPool.remove(m_usedIDPool.indexOf(node->getNodeID()));
        delete node;
    }
}

NetworkLayer::NetworkLayer(const NetworkLayer &other)
{
    if(this != &other)
    {
        if(!m_nodes.empty())
        {
            for(NetworkNode *node : m_nodes)
            {
                delete node;
            }
            m_nodes.clear();
        }
        for(NetworkNode *node : other.m_nodes)
        {
            NetworkNode *new_node = copyNetworkNode(node);
            if(new_node != nullptr)
            {
                m_nodes.push_back(new_node);
            }
        }
    }
}

NetworkLayer::NetworkLayer(NetworkLayer &&other)
{
    if(this != &other)
    {
        if(!m_nodes.empty())
        {
            for(NetworkNode *node : m_nodes)
            {
                delete node;
            }
            m_nodes.clear();
        }
        for(NetworkNode *node : other.m_nodes)
        {
            NetworkNode *new_node = node;
            if(new_node != nullptr)
            {
                m_nodes.push_back(new_node);
                node = nullptr;
            }
        }
    }
}

NetworkLayer& NetworkLayer::operator=(const NetworkLayer &other)
{
    if(this != &other)
    {
        if(!m_nodes.empty())
        {
            for(NetworkNode *node : m_nodes)
            {
                delete node;
            }
            m_nodes.clear();
        }
        for(NetworkNode *node : other.m_nodes)
        {
            NetworkNode *new_node = copyNetworkNode(node);
            if(new_node != nullptr)
            {
                m_nodes.push_back(new_node);
            }
        }
    }
    return *this;
}

NetworkLayer& NetworkLayer::operator=(NetworkLayer &&other)
{
    if(this != &other)
    {
        if(!m_nodes.empty())
        {
            for(NetworkNode *node : m_nodes)
            {
                delete node;
            }
            m_nodes.clear();
        }
        for(NetworkNode *node : other.m_nodes)
        {
            NetworkNode *new_node = node;
            if(new_node != nullptr)
            {
                m_nodes.push_back(new_node);
                node = nullptr;
            }
        }
    }
    return *this;
}

qint16 NetworkLayer::getLayerId() const
{
    return m_layer_id;
}

quint16 NetworkLayer::getNumOfNodes() const
{
    return m_nodes.size();
}

NetworkNode *NetworkLayer::getNode(quint16 id) const
{
    NetworkNode *retNode = nullptr;
    for(NetworkNode *node : m_nodes)
    {
        if(node->getNodeID() == id)
        {
            retNode = node;
            break;
        }
    }
    return retNode;
}

QVector<NetworkNode*>::const_iterator NetworkLayer::getIteratorToFirstNode()
{
    return m_nodes.constBegin();
}

NetworkNode* NetworkLayer::createNode(NetworkNode::NodeType nodeType, quint16 node_id)
{
    NetworkNode *new_node = nullptr;
    if(checkIfIdAvailable(node_id))
    {      
        switch(nodeType)
        {
            case NetworkNode::NodeType::Sensor:
                new_node = new SensorNode(node_id);
                break;
            case NetworkNode::NodeType::Cluster:
                new_node = new ClusterNode(node_id);
                break;
            default:
                break;
        }
        if(new_node != nullptr)
        {
            new_node->setLayer(m_layer_id);
            m_nodes.push_back(new_node);
            m_usedIDPool.push_back(node_id);
        }
    }
    return new_node;
}

bool NetworkLayer::connectNodes(quint16 first_node, quint16 second_node)
{
    bool connected = false;
    if(first_node != second_node)
    {
        qint16 fNode_idx = checkIfHasNode(first_node);
        qint16 sNode_idx = checkIfHasNode(second_node);
        if(fNode_idx >=0 && sNode_idx >= 0)
        {
            NetworkNode *fNode = m_nodes[fNode_idx];
            NetworkNode *sNode = m_nodes[sNode_idx];
            //check if valid node types and not Sensor - Sensor connection
            if(!((fNode->getNodeType() == NetworkNode::NodeType::Sensor) && (sNode->getNodeType() == NetworkNode::NodeType::Sensor)))
            {
                if((fNode->getNodeType() != NetworkNode::NodeType::NoType) && (sNode->getNodeType() != NetworkNode::NodeType::NoType))
                {
                    if(fNode->connectToNode(sNode))
                    {
                        connected = true;
                    }
                }
            }
        }
    }
    return connected;
}

bool NetworkLayer::connectNodeWidget(quint16 node_id, QWidget *widget)
{
    bool connected = false;
    qint16 node_idx = checkIfHasNode(node_id);
    if(node_idx >= 0)
    {
        connected = m_nodes[node_idx]->connectToNodeWidget(widget);
    }
    return connected;
}

bool NetworkLayer::disconnectNodeWidget(quint16 node_id)
{
    bool disconnected = false;
    qint16 node_idx = checkIfHasNode(node_id);
    if(node_idx >= 0)
    {
        disconnected = m_nodes[node_idx]->disconnectFromWidget();
    }
    return disconnected;
}

bool NetworkLayer::removeNode(quint16 node_id)
{
    bool removedNode = false;
    qint16 node_idx = checkIfHasNode(node_id);
    if(node_idx >= 0 && m_nodes[node_idx])
    {
        if(m_nodes[node_idx]->getNodeType() == NetworkNode::NodeType::Cluster)
        {
            if(sendRemovedMsg(m_nodes[node_idx]))
            {
                //reassign all connected sensor nodes
                reassignSensorNodes(node_id);
                m_nodes[node_idx]->disconnectFromNetwork();
            }
        }
        else if(m_nodes[node_idx]->getNodeType() == NetworkNode::NodeType::Sensor)
        {
            SensorNode *sensor = static_cast<SensorNode*>(m_nodes[node_idx]);
            if(sensor && sensor->isConnectedToCluster())
            {
                ClusterNode *cluster = static_cast<ClusterNode*>(getNode(sensor->getClusterID()));
                if(cluster)
                {
                    sensor->disconnectFromNode(cluster);
                    cluster->removeSensor(sensor);
                }
            }
        }
        m_nodes[node_idx]->disconnectFromWidget();
        m_usedIDPool.remove(m_usedIDPool.indexOf(node_id));
        delete m_nodes[node_idx];
        m_nodes.remove(node_idx);
        removedNode = true;

    }
    return removedNode;
}

bool NetworkLayer::moveNode(quint16 node_id, QPoint position)
{
    bool movedNode = false;
    qint16 node_idx = checkIfHasNode(node_id);
    if(node_idx >= 0 && m_nodes[node_idx])
    {
        m_nodes[node_idx]->setNodePosition(position);
        if(m_nodes[node_idx]->getNodeType() == NetworkNode::NodeType::Cluster)
        {
            ClusterNode *cluster = static_cast<ClusterNode*>((m_nodes[node_idx]));
            if(cluster)
            {
                quint16 numOfLeftConnectedClusters = cluster->getNumOfConnectedNodes();
                //before updating connections temporarly disconnect it from network
                //and set other cluster node paths to be different
                if(sendRemovedMsg(cluster))
                {
                    for(NetworkNode* node : m_nodes)
                    {
                        if(node->getNodeID() != node_id)
                        {
                            if(node->getNodeType() == NetworkNode::NodeType::Cluster)
                            {
                                if(cluster->checkIfInRange(node->getNodePosition()) && node->checkIfInRange(position))
                                {
                                    if(!cluster->checkIfConnectedToNode(node))
                                    {
                                        cluster->connectToNode(node);
                                        ++numOfLeftConnectedClusters;
                                    }
                                }
                                else
                                {
                                    if(cluster->checkIfConnectedToNode(node))
                                    {
                                        cluster->disconnectFromNode(node);
                                        --numOfLeftConnectedClusters;
                                    }
                                }
                            }
                            else if(node->getNodeType() == NetworkNode::NodeType::Sensor)
                            {
                                SensorNode *sensor = static_cast<SensorNode*>(node);
                                if(sensor)
                                {
                                    double distance = node->getDistanceFromNode(position);
                                    if(sensor->isConnectedToCluster())
                                    {
                                        if(distance < sensor->getDistanceFromNode(sensor->getClusterPosition()))
                                        {
                                            auto oldCluster = getNode(sensor->getClusterID());
                                            if(oldCluster)
                                            {
                                                sensor->disconnectFromNode(oldCluster);
                                                sensor->connectToNode(cluster);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if(sensor->checkIfInRange(position) && cluster->checkIfInRange(sensor->getNodePosition()))
                                        {
                                            sensor->connectToNode(cluster);
                                        }
                                    }

                                }
                            }
                        }
                    }
                    if(numOfLeftConnectedClusters == 0)
                    {
                        reassignSensorNodes(node_id);
                    }
                    else
                    {
                        cluster->sendSinkPathReq();
                    }
                    movedNode = true;
                }
            }
        }
        else if(m_nodes[node_idx]->getNodeType() == NetworkNode::NodeType::Sensor)
        {
            SensorNode *sensor = static_cast<SensorNode*>((m_nodes[node_idx]));
            if(sensor)
            {
                double minDistance = UINT64_MAX;
                NetworkNode *closestCluster = nullptr;
                for(NetworkNode* node : m_nodes)
                {
                    if(node->getNodeID() != node_id)
                    {
                        if(node->getNodeType() == NetworkNode::NodeType::Cluster)
                        {
                            double distance = node->getDistanceFromNode(position);
                            bool inRange = ((distance <= sensor->getNodeRange()) && node->checkIfInRange(position));
                            if(inRange && distance < minDistance)
                            {
                                minDistance = distance;
                                closestCluster = node;
                            }
                        }
                    }
                }
                auto oldCluster = getNode(sensor->getClusterID());
                if(closestCluster)
                {
                    if(oldCluster)
                    {
                        sensor->disconnectFromNode(oldCluster);
                    }
                    sensor->connectToNode(closestCluster);
                }
                else
                {
                    if(oldCluster)
                    {
                        sensor->disconnectFromNode(oldCluster);
                    }
                }
                movedNode = true;
            }
        }
    }
    return movedNode;
}

void NetworkLayer::setLayerId(quint16 id)
{
    m_layer_id = id;
}

qint16 NetworkLayer::checkIfHasNode(quint16 node_id) const
{
    qint16 node_idx = -1;
    qint16 i = 0;
    for(auto & node : m_nodes)
    {
        if(node->getNodeID() == node_id)
        {
            node_idx = i;
            break;
        }
        ++i;
    }
    return node_idx;
}

NetworkNode* NetworkLayer::copyNetworkNode(const NetworkNode *node) const
{
    NetworkNode *new_node = nullptr;
    switch(node->getNodeType())
    {
        case NetworkNode::Sensor:
            new_node = new SensorNode(*(reinterpret_cast<const SensorNode*>(node)));
            break;
        case NetworkNode::Cluster:
            new_node = new ClusterNode(*(reinterpret_cast<const ClusterNode*>(node)));
            break;
        default:
            break;
    }
    return new_node;
}

QByteArray NetworkLayer::createNodeRemovalMsg(quint16 node_id) const
{
    QByteArray msg;
    ClusterNode *node = static_cast<ClusterNode*>(getNode(node_id));
    if(node)
    {
        QJsonObject removeNodeObj =
        {
            {NODE_ID, node_id},
            {LAYER_ID, m_layer_id},
            {NODE_POSITION_X, node->getNodePosition().x()},
            {NODE_POSITION_Y, node->getNodePosition().y()},
            {NODE_STATE, node->getCurrentState()}
        };
        QJsonDocument jsonMsg(removeNodeObj);
        msg = jsonMsg.toBinaryData();
    }
    return msg;
}

void NetworkLayer::reassignSensorNodes(quint16 node_id)
{
    ClusterNode *cluster = static_cast<ClusterNode*>(getNode(node_id));
    quint16 i = 0;
    quint16 numOfSensors = cluster->getNumOfSensors();
    for(QVector<NetworkNode*>::const_iterator sensor = cluster->getIteratorToFirstSensor(); i < numOfSensors; sensor++)
    {
        i++;
        double minDistance = UINT64_MAX;
        NetworkNode *closestCluster = nullptr;
        (*sensor)->disconnectFromNode(cluster);
        for(auto && node : m_nodes)
        {
            if(node->getNodeType() == NetworkNode::NodeType::Cluster)
            {
                if(node->getNodeID() != node_id)
                {
                    double distance = node->getDistanceFromNode((*sensor)->getNodePosition());
                    if(distance <= (*sensor)->getNodeRange() && distance < minDistance)
                    {
                        minDistance = distance;
                        closestCluster = node;
                    }
                }
            }
        }
        if(closestCluster && minDistance < UINT64_MAX)
        {
            //new cluster found to connect
            (*sensor)->connectToNode(closestCluster);
        }
    }
}

bool NetworkLayer::sendRemovedMsg(NetworkNode *clusterNode)
{
    bool success = false;
    ClusterNode *cluster = static_cast<ClusterNode*>(clusterNode);
    if(cluster)
    {
        DataFrame frame;
        QByteArray nodeRemovedMsg = createNodeRemovalMsg(cluster->getNodeID());
        if(!nodeRemovedMsg.isEmpty())
        {
            frame.setMsg(nodeRemovedMsg);
            frame.setSender(qMakePair(cluster->getNodeID(), m_layer_id));
            if(cluster->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK)
            {
                //broadcast to all nodes that this cluster is not available
                ClusterNode::resetBroadCastSyncVector(m_layer_id);
                frame.setMsgType(DataFrame::RxData::DIRECT_CLUSTER_REMOVED);
                cluster->sendData(frame);
                //sink sends new paths to all nodes that are connected
                //else it sets all of their states to disconnected
                frame.setMsgType(DataFrame::RxData::REMOVED_NODE);
                cluster->sendDataToSink(frame);
            }
            else if(cluster->getCurrentState() == ClusterNode::ClusterStates::CONNECTED)
            {
                frame.setMsgType(DataFrame::RxData::REMOVED_NODE);
                frame.setPath(cluster->getSinkPath());
                frame.setDestination(qMakePair(cluster->getSinkPath()[0], m_layer_id));
                cluster->sendData(frame);
            }
            success = true;
            cluster->setStateDisconnected();
        }
    }
    return success;
}

bool NetworkLayer::checkIfIdAvailable(quint16 id)
{
    return (m_usedIDPool.indexOf(id) < 0);
}
