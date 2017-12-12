#include "NetworkLayer.h"
#include "NetworkNode.h"
#include "SensorNode.h"
#include "ClusterNode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <memory>

NetworkLayer::NetworkLayer(qint16 layer_id):
    m_layer_id(layer_id)
{

}

NetworkLayer::~NetworkLayer()
{
    for(NetworkNode *node : m_nodes)
    {
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

NetworkNode* NetworkLayer::createNode(NetworkNode::NodeType nodeType, quint16 node_id)
{
    NetworkNode *new_node = nullptr;
    if(checkIfHasNode(node_id) < 0)
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
    if(node_idx >= 0)
    {
        DataFrame nodeRemovedMsg = createNodeRemovalMsg(node_id);
        m_nodes[node_idx]->sendData(nodeRemovedMsg);
        m_nodes[node_idx]->disconnectFromWidget();
        m_nodes[node_idx]->disconnectFromNetwork();
        delete m_nodes[node_idx];
        m_nodes.remove(node_idx);
        removedNode = true;
    }
    return removedNode;
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

DataFrame NetworkLayer::createNodeRemovalMsg(quint16 node_id) const
{
    bool created = false;
    ClusterNode *node = static_cast<ClusterNode*>(getNode(node_id));
    if(node)
    {
        QJsonObject removeNodeObj =
        {
            {ClusterNode::NODE_ID, node_id},
            {ClusterNode::LAYER_ID, m_layer_id},
            {ClusterNode::NODE_POSITION_X, node->getNodePostion().x()},
            {ClusterNode::NODE_POSITION_Y, node->getNodePostion().y()},
            {ClusterNode::NODE_STATE, node->getCurrentState()}
        };
        QJsonDocument jsonMsg(removeNodeObj);
        msg = jsonMsg.toBinaryData();
        if(!msg.isEmpty() && !msg.isNull())
        {
            created = true;
        }
        created = true;
    }
    return created;
}
