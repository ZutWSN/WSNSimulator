#include "ClusterNode.h"
#include "SensorNode.h"

ClusterNode::ClusterNode(quint16 node_id) :
    NetworkNode(node_id)
{

}

bool ClusterNode::addSensorNode(SensorNode *sensor)
{
    bool success = false;
    if(sensor)
    {
        if(!checkIfSensorConnected(sensor->getNodeID()))
        {
            m_sensors.push_back(sensor);
            success = true;
        }
    }
    return success;
}
bool ClusterNode::connectToNode(NetworkNode *node)
{
    //check if cluster or sensor, add to separate vectors - sensors and neighbours
    bool connected = false;
    if(node)
    {
        if(m_layer_id >= 0)
        {
            if(node->getNodeLayer() == m_layer_id)
            {
                if(node->getNodeID() != m_node_id)
                {
                    //both nodes have to be in each others range for 2 way communication
                    if(checkIfInRange(node->getNodePostion()) && node->checkIfInRange(m_node_position))
                    {
                        connected = static_cast<bool>(connect(this, SIGNAL(dataSend(DataFrame)), node, SLOT(onReceivedData(DataFrame))));
                        connected &= static_cast<bool>(connect(node, SIGNAL(dataSend(DataFrame)), this, SLOT(onReceivedData(DataFrame))));
                        m_connectedNodeID = node->getNodeID();
                        m_connectedToNode = true;
                    }
                }
            }
        }
    }
    return connected;
}

bool NetworkNode::connectToNode(NetworkNode *node)
{
    bool connected = false;
    if(node)
    {
        if(m_layer_id >= 0)
        {
            if(node->getNodeLayer() == m_layer_id)
            {
                if(node->getNodeID() != m_node_id)
                {
                    //both nodes have to be in each others range for 2 way communication
                    if(checkIfInRange(node->getNodePostion()) && node->checkIfInRange(m_node_position))
                    {
                        connected = static_cast<bool>(connect(this, SIGNAL(dataSend(DataFrame)), node, SLOT(onReceivedData(DataFrame))));
                        connected &= static_cast<bool>(connect(node, SIGNAL(dataSend(DataFrame)), this, SLOT(onReceivedData(DataFrame))));
                        m_connectedNodeID = node->getNodeID();
                        m_connectedToNode = true;
                    }
                }
            }
        }
    }
    return connected;
}

bool ClusterNode::addNeighbourCluster(ClusterNode *cluster)
{
    bool success = false;
    if(cluster && (cluster != this))
    {
        if(connectToNode(static_cast<NetworkNode*>(cluster)))
        {
            m_clusterNeighbours.push_back(cluster);
            cluster->m_clusterNeighbours.push_back(this);
            success = true;
        }
    }
    return success;
}

NetworkNode::NodeType ClusterNode::getNodeType() const
{
    return NodeType::Cluster;
}

bool ClusterNode::checkIfSensorConnected(quint16 sensor_id)
{
    bool connected = false;
    for(SensorNode* sensor : m_sensors)
    {
        if(sensor->getNodeID() == sensor_id)
        {
            connected = true;
            break;
        }
    }
    return connected;
}

