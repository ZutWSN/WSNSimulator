#include "ClusterNode.h"
#include "SensorNode.h"

ClusterNode::ClusterNode(quint16 node_id) :
    NetworkNode(node_id)
{

}

bool ClusterNode::addSensorNode(quinit16 sensor_id)
{
    bool success = false;
    if(sensor)
    {
        if(!checkIfSensorConnected(sensor_id))
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
    //add definition of virtual function in sensor, also disconnect
    bool connected = false;
    if(node)
    {
        //check if cluster of sensor and handle accordingly
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
    for(auto && id : m_sensors)
    {
        if(sensor_id == sensor_id)
        {
            connected = true;
            break;
        }
    }
    return connected;
}

