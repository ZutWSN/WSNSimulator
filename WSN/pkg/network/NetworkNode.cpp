#include "NetworkNode.h"
#include "DragWidget.h"
#include <QPair>

NetworkNode::NetworkNode(quint16 node_id) :
    m_range(0),
    m_node_id(node_id),
    m_layer_id(-1),
    m_node_position(QPoint(0, 0)),
    m_Widget(nullptr),
    m_connectedToWidget(false)
{

}

NetworkNode::NetworkNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position):
    m_range(range),
    m_node_id(node_id),
    m_layer_id(layer_id),
    m_node_position(node_position),
    m_Widget(nullptr),
    m_connectedToWidget(false)
{

}

NetworkNode::NetworkNode(const NetworkNode &other)
{
    if(this != &other)
    {
        m_layer_id = other.m_layer_id;
        m_node_id = other.m_node_id;
        m_node_position = other.m_node_position;
        m_range = other.m_range;
        m_connectedToWidget = connectToNodeWidget(other.m_Widget);
        m_Widget = (m_connectedToWidget) ? other.m_Widget : nullptr;
        m_sendDataReceived = (m_connectedToWidget) ? other.m_sendDataReceived : false;
    }
}

NetworkNode& NetworkNode::operator=(const NetworkNode &other)
{
    if(this != &other)
    {
        m_layer_id = other.m_layer_id;
        m_node_id = other.m_node_id;
        m_node_position = other.m_node_position;
        m_range = other.m_range;
        m_connectedToWidget = connectToNodeWidget(other.m_Widget);
        m_Widget = (m_connectedToWidget) ? other.m_Widget : nullptr;
        m_sendDataReceived = (m_connectedToWidget) ? other.m_sendDataReceived : false;
    }
    return *this;
}

NetworkNode::~NetworkNode()
{

}

bool NetworkNode::sendData(const DataFrame &txData)
{
    bool result = false;
    if(!txData.frameEmpty())
    {
        emit dataSend(txData);
        result = true;
        m_pendingSendDataFrames(txData);
    }
    return result;
}

bool NetworkNode::processReceiveAcknowledged(const DataFrame &rxData)
{
    //check if frame in pending data, if it is remove it and send signal
    //if all pending data has been removed
    bool deletedPending = false;
    for(quint16 i = 0; i < m_pendingSendDataFrames.size(); i++)
    {
        if(m_pendingSendDataFrames[i].getSender() == rxData.getSender())
        {
            m_pendingSendDataFrames.remove(i);
            deletedPending = true;
        }
    }
    return deletedPending;
}

bool NetworkNode::addNode(NetworkNode *node)
{
    bool success = false;
    if(node && (m_layer_id == node->getNodeLayer()))
    {
        if(m_connectedNodes.indexOf(node) == -1)
        {
            m_connectedNodes.push_back(node);
            success = true;
        }
    }
    return success;
}

void NetworkNode::setLayer(qint16 layer_id)
{
    m_layer_id = layer_id;
}

bool NetworkNode::connectToNodeWidget(QWidget *widget)
{
    bool success = false;
    DragWidget *dragWidget = static_cast<DragWidget*>(widget);
    if(dragWidget && !dragWidget->isConnectedToNode())
    {
        if(!m_connectedToWidget)
        {
            success = static_cast<bool>(connect(this, SIGNAL(receivedNewData(DataFrame)), dragWidget, SLOT(onNodeReceivedData(DataFrame))));
            m_connectedToWidget = success;
            if(m_connectedToWidget)
            {
                dragWidget->setConnectedToNode(true);
                m_Widget = widget;
            }
        }
    }
    return success;
}

bool NetworkNode::disconnectFromWidget()
{
    bool disconnected = false;
    if(m_connectedToWidget && m_Widget)
    {
        DragWidget *dragWidget = static_cast<DragWidget*>(m_Widget);
        if(dragWidget->isConnectedToNode())
        {
            disconnected = disconnect(this, 0, m_Widget, 0);
        }
        if(disconnected)
        {
            dragWidget->setConnectedToNode(false);
            m_Widget = nullptr;
            m_connectedToWidget = false;           
        }
    }
    return disconnected;
}

void NetworkNode::setNodePosition(const QPoint &position)
{
    m_node_position = position;
}

quint16 NetworkNode::getNodeID() const
{
    return m_node_id;
}

qint16 NetworkNode::getNodeLayer() const
{
    return m_layer_id;
}

QPoint NetworkNode::getNodePostion() const
{
    return m_node_position;
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
                    //check if already connected to this node
                    if(!checkIfConnectedToNode(node))
                    {
                        //both nodes have to be in each others range for 2 way communication
                        if(checkIfInRange(node->getNodePostion()) && node->checkIfInRange(m_node_position))
                        {
                            connected = static_cast<bool>(connect(this, SIGNAL(dataSend(DataFrame)), node, SLOT(onReceivedData(DataFrame))));
                            connected &= static_cast<bool>(connect(node, SIGNAL(dataSend(DataFrame)), this, SLOT(onReceivedData(DataFrame))));
                            connected &= node->addNode(this);
                            if(connected)
                            {
                                m_connectedNodes.push_back(node);

                            }
                        }
                    }
                }
            }
        }
    }
    return connected;
}

bool NetworkNode::disconnectFromNode(NetworkNode *node)
{
    bool disconnected = false;
    if(node)
    {
        if(checkIfConnectedToNode(node))
        {
            disconnected = disconnect(this, 0, node, 0);
            disconnected &= disconnect(node, 0, this, 0);
            if(disconnected)
            {
                m_connectedNodes.remove(m_connectedNodes.indexOf(node));
            }
        }
    }
    return disconnected;
}

bool NetworkNode::getSendDataReceived() const
{
    return m_sendDataReceived;
}

NetworkNode::NodeType NetworkNode::getNodeType() const
{
    return NodeType::NoType;
}

bool NetworkNode::checkIfInRange(const QPoint &position) const
{
    quint16 dist = pow((pow(abs(position.x() - m_node_position.x()), 2) + pow(abs(position.y() - m_node_position.y()), 2)), 0.5);
    return dist <= m_range ;
}

void NetworkNode::setNodeID(quint16 node_id)
{
    m_node_id = node_id;
}

void NetworkNode::onReceivedData(const DataFrame &rxData)
{
    //Get info from data frame
    switch(rxData.getMsgType())
    {
        case DataFrame::RxData::NEW_DATA :
        case DataFrame::RxData::SENSOR_BROADCAST :
        case DataFrame::RxData::PATH_SYNC :
            processNewData(rxData);
            break;
        case DataFrame::RxData::RECEIVED_DATA : //receiver notification that last send data was accepted
            processReceiveAcknowledged(rxData);
            break;
        case DataFrame::RxData::NO_DATA:
        default:
            break;
    }
}

void NetworkNode::processNewData(const DataFrame &rxData)
{
    //notify corresponding widget about received data
    emit receivedNewData(rxData);
    //send back that data received
    DataFrame txData(rxData);
    txData.setMsgType(DataFrame::RxData::RECEIVED_DATA);
    sendData(txData);
}

bool NetworkNode::checkIfConnectedToNode(NetworkNode *node) const
{
    bool connected = false;
    for(NetworkNode* connected_node : m_connectedNodes)
    {
        if(node == connected_node)
        {
            connected = true;
            break;
        }
    }
    return connected;
}

bool NetworkNode::checkIfConnectedToNode(const QPair<quint16, quint16> &node) const
{
    bool connected = false;
    for(NetworkNode* connected_node : m_connectedNodes)
    {
        if(node.first == connected_node->getNodeID() &&
           node.second == connected_node->getNodeID())
        {
            connected = true;
            break;
        }
    }
    return connected;
}

quint16 NetworkNode::getNumOfPendingDataFrames() const
{
    return m_pendingSendDataFrames.size();
}


