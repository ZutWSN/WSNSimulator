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

