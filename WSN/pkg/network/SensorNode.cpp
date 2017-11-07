#include "SensorNode.h"
#include "ClusterNode.h"

SensorNode::SensorNode(quint16 node_id):
    NetworkNode(node_id),
    m_cluster_id(0),
    m_connectedToCluster(false)
{

}

bool SensorNode::connectToCluster(ClusterNode *cluster)
{
    bool connected  = false;
    if(cluster && !m_connectedToCluster)
    {
        if(cluster->addSensorNode(this))
        {
            NetworkNode *cluter_node = cluster;
            if(connectToNode(cluster_node))
            {
                connected = true;
                m_connectedToCluster = true;
            }
        }
    }
    return connected;
}

bool SensorNode::disconnectFromCluster()
{
    bool success = false;
    if(m_connectedToCluster)
    {

    }
    return success;
}

NetworkNode::NodeType SensorNode::getNodeType() const
{
    return NodeType::Sensor;
}

bool SensorNode::isConnectedToCluster() const
{
    return m_connectedToCluster;
}
