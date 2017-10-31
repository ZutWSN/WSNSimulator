#include "NetworkLayer.h"
#include "NetworkNode.h"

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

NetworkLayer::NetworkLayer(const SensorNetwork &other)
{
    if(this != other)
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

NetworkLayer::NetworkLayer(SensorNetwork &&other)
{
    if(this != other)
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
            NetworkNode *new_node = *node;
            if(new_node != nullptr)
            {
                m_nodes.push_back(new_node);
                node = nullptr;
            }
        }
    }
}

NetworkLayer& NetworkLayer::operator=(const SensorNetwork &other)
{
    if(this != other)
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

NetworkLayer& NetworkLayer::operator=(SensorNetwork &&other)
{
    if(this != other)
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
            NetworkNode *new_node = *node;
            if(new_node != nullptr)
            {
                m_nodes.push_back(new_node);
                node = nullptr;
            }
        }
    }
    return *this;
}

NetworkLayer::~NetworkLayer()
{

}

NetworkLayer::NetworkLayer(const NetworkLayer &other)
{

}

NetworkLayer::NetworkLayer(const NetworkLayer &&other)
{

}

NetworkLayer &NetworkLayer::operator=(const NetworkLayer &other)
{

}

NetworkLayer &NetworkLayer::operator=(const NetworkLayer &&other)
{

}

bool NetworkLayer::addNode(NetworkNode *node)
{
    bool addedNode = false;
    if(-1 == checkIfHasNode(node->getNodeID())) //node not found
    {
        node->setLayer(m_layer_id);
        m_nodes.push_back(node);
        addedNode = true;
    }
    return addedNode;
}

bool NetworkLayer::removeNode(NetworkNode *node)
{
    bool removedNode = false;
    qint16 node_idx = checkIfHasNode(node->getNodeID());
    if(0 <= node_idx)
    {
        node->setLayer(-1);
        m_nodes.remove(node_idx);
        removedNode = true;
    }
    return removedNode;
}

qint16 NetworkLayer::getLayerId() const
{
    return m_layer_id;
}

bool NetworkLayer::createNode(NetworkNode::NodeType nodeType, quint16 node_id = 0)
{
    bool addedNewNode = false;
    if(checkIfHasNode(node_id))
    {
        NetworkNode *new_node = nullptr;
        switch(nodeType)
        {
            case NetworkNode::Sensor:
                new_node = new SensorNode(node_id);
                break;
            case NetworkNode::Cluster:
                new_node = new ClusterNode(node_id);
                break;
            case NetworkNode::Sink:
                new_node = new SinkNode();
                break;
        }
        if(new_node != nullptr)
        {
            m_nodes.push_back(new_node);
            addedNewNode = true;
        }
    }
    return addedNewNode;
}

NetworkNode *NetworkLayer::getNetworkNode(quint16 node_id) const
{
    NetworkNode *node = nullptr;
    if(checkIfHasNode(node_id))
    {
        node = m_nodes[node_id];
    }
    return node;
}

bool NetworkLayer::checkIfHasNode(quint16 node_id) const
{
    bool hasNode = false;
    for(auto & node : m_nodes)
    {
        if(node->getNodeID() == node_id)
        {
            hasNode = true;
            break;
        }
    }
    return hasNode;
}

NetworkNode* NetworkLayer::copyNetworkNode(const NetworkNode *node) const
{
    NetworkNode *new_node = nullptr;
    switch(node->getNodeType())
    {
    case NetworkNode::Sensor:
        new_node = new SensorNode(*node);
        break;
    case NetworkNode::Cluster:
        new_node = new ClusterNode(*node);
        break;
    case NetworkNode::Sink:
        new_node = new SinkNode(*node);
        break;
    }
    return new_node;
}
