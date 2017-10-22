#include "NetworkNode.h"

NetworkNode::NetworkNode() :
    m_sendDataReceived(false)
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

void NetworkNode::onReceivedData(const DataFrame &rxData)
{
    //Get info from data frame
    switch(rxData.getMsgType())
    {
        case DataFrame::RxData::NEW_DATA : // received new data, later add checking it and sending back info that it was recived sucessfully
            break;
        case DataFrame::RxData::RECEIVED_DATA :
            m_sendDataReceived = true; // later create send data received successfull by target data pool, which will be changed and checked with send was acknowledged
            break;
        default:
            break;
    }
}


