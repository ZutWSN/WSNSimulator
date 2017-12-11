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
                    if(checkIfInRange(node->getNodePostion()) && node->checkIfInRange(m_node_position))
                    {
                        ClusterNode *cluster = static_cast<ClusterNode*>(node);
                        connected = static_cast<bool>(connect(cluster, SIGNAL(broadcastDataToSensors(DataFrame)), this, SLOT(onReceivedDataFromCluster(DataFrame))));
                        connected &= static_cast<bool>(connect(this, SIGNAL(clusterDataSend(QByteArray)), cluster, SLOT(onReceivedDataFromSensor(QByteArray))));
                        connected &= cluster->addNode(this);
                        if(connected)
                        {
                            m_connectedToCluster = true;
                            m_cluster_id = node->getNodeID();
                        }
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
                    disconnected &= disconnect(this, 0, node, 0);
                    if(disconnected)
                    {
                        m_connectedToCluster = false;
                        m_cluster_id = 0;
                    }
                }
            }
        }
    }
    return disconnected;
}

bool SensorNode::sendDataToCluster(const QByteArray &data)
{
    bool dataSend = false;
    if(!data.isEmpty())
    {
        dataSend = true;
        emit clusterDataSend(data);
    }
    return dataSend;
}

NetworkNode::NodeType SensorNode::getNodeType() const
{
    return NodeType::Sensor;
}

bool SensorNode::isConnectedToCluster() const
{
    return m_connectedToCluster;
}

void SensorNode::onReceivedDataFromCluster(const DataFrame &rxData)
{
    if(rxData.getMsgType() == DataFrame::RxData::SENSOR_BROADCAST)
    {
        if(rxData.getSender().first == m_cluster_id && rxData.getSender().second == m_layer_id)
        {
            processNewData(rxData);
        }
    }
}

void SensorNode::processNewData(const DataFrame &rxData)
{
    //check data - process frame information - > will
    // later decided what to do with it
    //base class notifies connected widget
    NetworkNode::processNewData(rxData);

}

