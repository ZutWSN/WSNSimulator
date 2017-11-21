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

NetworkNode::NodeType ClusterNode::getNodeType() const
{
    return NodeType::Cluster;
}


void ClusterNode::processNewData(const DataFrame &rxData)
{
    NetworkNode::processNewData(rxData);
    QPair<quint16, quint16> destination = rxData.getDestination();
    if(destination.second == m_layer_id)
    {
        if(destination.first == m_node_id)
        {
            //send back that message received
            DataFrame txData(rxData);
            //now start processing for forwarding or broadcasting
            txData.setSender(qMakePair(m_node_id, m_layer_id));
            if(rxData.isFinalDestination())
            {
                txData.setMsgType(DataFrame::RxData::SENSOR_BROADCAST);
                emit broacastDataToSensors(rxData);
            }
            else
            {
                QPair<quint16, quint16> nextNode = rxData.getNextChainNode(m_node_id);
                txData.setDestination(nextNode);
                txData.setMsgType(DataFrame::RxData::NEW_DATA);
                sendData(txData);
            }
        }
    }
}

