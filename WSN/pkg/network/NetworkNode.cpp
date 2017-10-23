#include "NetworkNode.h"

NetworkNode::NetworkNode(quint16 node_id, quint16 layer_id) :
    m_sendDataReceived(false),
    m_node_id(node_id),
    m_layer_id(layer_id)
{

}

bool NetworkNode::sendData(const DataFrame &txData)
{
    bool result = false;
    if(!txData.frameEmpty())
    {
        m_sendDataReceived = false;
        emit dataSend(txData);
        result = true;
    }
    return result;
}

void NetworkNode::setLayer(quint16 layer_id)
{
    m_layer_id = layer_id;
}

bool NetworkNode::connectToNode(NetworkNode *node)
{
    bool connected = false;
    if(node->getNodeLayer() == m_layer_id)
    {
        if(node->getNodeID() != m_node_id)
        {
            connected = static_cast<bool>(connect(this, SIGNAL(dataSend(DataFrame)), node, SLOT(onReceivedData(DataFrame))));
            connected &= static_cast<bool>(connect(node, SIGNAL(dataSend(DataFrame)), this, SLOT(onReceivedData(DataFrame))));
        }
    }
    return connected;
}

bool NetworkNode::getSendDataReceived() const
{
    return m_sendDataReceived;
}

bool NetworkNode::setNodeID(quint16 node_id)
{
    m_node_id = node_id;
}

void NetworkNode::onReceivedData(const DataFrame &rxData)
{
    //Get info from data frame
    switch(rxData.getMsgType())
    {
        case DataFrame::RxData::NEW_DATA : // received new data, later add checking it and sending back info that it was recived sucessfully
            processData(rxData);
            break;
        case DataFrame::RxData::RECEIVED_DATA :
            m_sendDataReceived = true; // later create send data received successfull by target data pool, which will be changed and checked with send was acknowledged
            break;
        default:
            break;
    }
}

void NetworkNode::processData(const DataFrame &rxData)
{

}


