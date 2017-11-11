#include "SensorNode.h"
#include "ClusterNode.h"

SensorNode::SensorNode(quint16 node_id):
    NetworkNode(node_id),
    m_cluster_id(0),
    m_connectedToCluster(false)
{

}

//add other network nodes overloaded contructor

bool SensorNode::connectToNode(NetworkNode *node)
{
    // only allows connecting to clusters
    bool connected  = false;
    if(node && (node->getNodeType() == NetworkNode::NodeType::Cluster))
    {
        if(!m_connectedToCluster)
        {
            if(NetworkNode::connectToNode(node))
            {
                static_cast<ClusterNode*>(node)->addSensorNode(this);
                connected = true;
                m_connectedToCluster = true;
                m_cluster_id = node->getNodeID();
            }
        }
    }
    return connected;
}

bool SensorNode::disconnectFromNode(NetworkNode *node)
{
    bool disconnected = false;
    if(node && (node->getNodeType() == NetworkNode::NodeType::Cluster))
    {
        if(m_connectedToCluster)
        {
            if(NetworkNode::disconnectFromNode(node))
            {
                m_cluster_id = 0;
                m_connectedToCluster = false;
            }
        }
    }
    return disconnected;
}

NetworkNode::NodeType SensorNode::getNodeType() const
{
    return NodeType::Sensor;
}

bool SensorNode::isConnectedToCluster() const
{
    return m_connectedToCluster;
}
