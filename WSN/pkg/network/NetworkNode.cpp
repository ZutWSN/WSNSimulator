#include "NetworkNode.h"
#include "DragWidget.h"

NetworkNode::NetworkNode(quint16 node_id) :
    m_range(0),
    m_node_id(node_id),
    m_layer_id(-1),
    m_node_position(QPoint(0, 0)),
    m_connectedNodeID(0),
    m_Widget(nullptr),
    m_connectedToNode(false),
    m_sendDataReceived(false),
    m_connectedToWidget(false)
{

}

NetworkNode::NetworkNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position):
    m_range(range),
    m_node_id(node_id),
    m_layer_id(layer_id),
    m_node_position(node_position),
    m_connectedNodeID(0),
    m_Widget(nullptr),
    m_connectedToNode(false),
    m_sendDataReceived(false),
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
        m_sendDataReceived = false;
        emit dataSend(txData);
        result = true;
    }
    return result;
}

void NetworkNode::setLayer(qint16 layer_id)
{
    m_layer_id = layer_id;
}

bool NetworkNode::connectToNodeWidget(QWidget *widget)
{
    bool success = false;
    DragWidget *dragWidget = static_cast<DragWidget*>(widget);
    if(dragWidget)
    {
        if(m_connectedToWidget)
        {
            if(disconnect(this, 0, 0, 0))
            {
                m_Widget = nullptr;
                m_connectedToWidget = false;
            }
        }
        success = static_cast<bool>(connect(this, SIGNAL(receivedNewData(DataFrame)), dragWidget, SLOT(onNodeReceivedData(DataFrame))));
    }
    m_connectedToWidget = success;
    if(m_connectedToWidget) {m_Widget = widget; }
    return success;
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
                    //both nodes have to be in each others range for 2 way communication
                    if(checkIfInRange(node->getNodePostion()) && node->checkIfInRange(m_node_position))
                    {
                        connected = static_cast<bool>(connect(this, SIGNAL(dataSend(DataFrame)), node, SLOT(onReceivedData(DataFrame))));
                        connected &= static_cast<bool>(connect(node, SIGNAL(dataSend(DataFrame)), this, SLOT(onReceivedData(DataFrame))));
                        m_connectedNodeID = node->getNodeID();
                        m_connectedToNode = true;
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
        if(node->getNodeID() == m_connectedNodeID)
        {
            disconnected = disconnect()
        }
    }
}

bool NetworkNode::getSendDataReceived() const
{
    return m_sendDataReceived;
}

NetworkNode::NodeType NetworkNode::getNodeType() const
{
    return NodeType::NoType;
}

bool NetworkNode::checkIfInRange(QPoint &position) const
{
    quint16 dist = pow((pow(abs(position.x() -m_node_position(x)), 2) + pow(abs(position.y() -m_node_position(y)), 2)), 0.5);
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
        case DataFrame::RxData::NEW_DATA : // received new data, later add checking it and sending back info that it was recived sucessfully
            processData(rxData);
            break;
        case DataFrame::RxData::RECEIVED_DATA : //receiver notification that last send data was accepted
            m_sendDataReceived = true; // later create send data received successfull by target data pool, which will be changed and checked with send was acknowledged
            break;
        default:
            break;
    }
}

void NetworkNode::processData(const DataFrame &rxData)
{
    //notify corresponding widget about received data
    emit receivedNewData(rxData);
}


