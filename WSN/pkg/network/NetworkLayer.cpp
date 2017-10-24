#include "NetworkLayer.h"
#include "NetworkNode.h"

NetworkLayer::NetworkLayer(qint16 layer_id):
    m_layer_id(layer_id)
{

}

NetworkLayer::~NetworkLayer()
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

qint16 NetworkLayer::checkIfHasNode(quint16 node_id) const
{
    qint16 node_idx = -1;
    quint16 i = 0;
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

