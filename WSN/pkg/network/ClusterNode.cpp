#include "ClusterNode.h"

ClusterNode::ClusterNode(quint16 node_id) :
    NetworkNode(node_id)
{

}

ClusterNode::ClusterNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position):
    NetworkNode(node_id, range, layer_id, node_position)
{

}

ClusterNode::~ClusterNode()
{

}


bool ClusterNode::broadCastDataToSensors() const
{
    //send Data to Sensors connected to cluster
    return true;
}

NetworkNode::NodeType ClusterNode::getNodeType() const
{
    return NodeType::Cluster;
}

ClusterNode::onReceivedData(const DataFrame &rxData)
{
    //handle checking if data is for this cluster if not do nothing - does not happen for sensor
    //so it doesnt overload this, only process data
}

void ClusterNode::processData(const DataFrame &rxData)
{
    //accumulate if this is its destination, else forward it
    //next node in defined path - emits signal to all connected cluster
    //neighbours, only one processes it and forwards further, rest ignores it after check.
    NetworkNode::processData(rxData);
}

