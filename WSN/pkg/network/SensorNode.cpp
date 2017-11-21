#include "SensorNode.h"
#include "ClusterNode.h"

SensorNode::SensorNode(quint16 node_id):
    NetworkNode(node_id),
    m_cluster_id(0),
    m_connectedToCluster(false)
{

}

SensorNode::SensorNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position) :
     NetworkNode(node_id, range, layer_id, node_position),
     m_cluster_id(0),
     m_connectedToCluster(false)
{

}

SensorNode::~SensorNode()
{

}

bool SensorNode::connectToNode(NetworkNode *node)
{
    // only allows connecting to clusters
    bool connected  = false;
    if(node && (node->getNodeType() == NetworkNode::NodeType::Cluster))
    {
        if(!m_connectedToCluster)
        {
            if(node->getNodeLayer() == m_layer_id)
            {
                if(node->getNodeID() != m_node_id)
                {
                    ClusterNode *cluster = static_cast<ClusterNode*>(node);
                    if(static_cast<bool>(connect(cluster, SIGNAL(broacastDataToSensors(DataFrame)), this, SLOT(onReceivedDataFromCluster(DataFrame)))))
                    {
                        connected = true;
                        m_connectedToCluster = true;
                        m_cluster_id = node->getNodeID();
                    }
                }
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
            if(node->getNodeLayer() == m_layer_id)
            {
                if(node->getNodeID() == m_cluster_id)
                {
                    disconnected = disconnect(node, 0, this, 0);
                    if(disconnected)
                    {
                        m_connectedNodes.remove(m_connectedNodes.indexOf(node));
                        m_connectedToCluster = false;
                        m_cluster_id = 0;
                    }
                }
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

void SensorNode::processNewData(const DataFrame &rxData)
{
    //check data - process frame information - > will
    // later decided what to do with it
    //base class notifies connected widget
    NetworkNode::processNewData(rxData);

}

