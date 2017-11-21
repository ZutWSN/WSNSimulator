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
            if(rxData.isFinalDestination(txData.getSender()))
            {
                if(rxData.getMsgType() == DataFrame::RxData::PATH_SYNC)
                {
                    bool extractNewPathFromMsg(rxData.getMsg());
                }
                else
                {
                    txData.setMsgType(DataFrame::RxData::SENSOR_BROADCAST);
                    emit broacastDataToSensors(rxData);
                }
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

bool ClusterNode::extractNewPathFromMsg(const QByteArray &pathMsg)
{
    /* extract new path from QByteArray message, define message text format
     * np. Parse this Json, it has all data regarding netwok paths
     * JSon:
     * {
        "Sink_ID": "0",
        "Paths": [
        { "Node_ID":"0", "Layer_ID":"2", "Path":[ "0", "1", "2" ] },
        { "Node_ID":"1", "Layer_ID":"2", "Path":[ "1", "2", "0" ] },
        { "Node_ID":"2", "Layer_ID":"2", "Path":[ "2", "1", "0" ] }
    */
    return true;
}

