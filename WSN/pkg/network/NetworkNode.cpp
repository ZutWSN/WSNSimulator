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
        m_pendingSendDataFrames.clear();
        if(m_connectedToWidget)
        {
            m_Widget = other.m_Widget;
            m_pendingSendDataFrames = m_pendingSendDataFrames;
        }
        else
        {
            m_Widget = nullptr;
        }
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
        m_pendingSendDataFrames.clear();
        if(m_connectedToWidget)
        {
            m_Widget = other.m_Widget;
            m_pendingSendDataFrames = m_pendingSendDataFrames;
        }
        else
        {
            m_Widget = nullptr;
        }
    }
    return *this;
}

NetworkNode::~NetworkNode()
{

}

bool NetworkNode::sendData(const DataFrame &txData)
{
    bool result = false;
    if(txData.getMsgType() != DataFrame::RxData::NO_DATA)
    {
        result = true;
        if(txData.getMsgType() != DataFrame::RxData::RECEIVED_DATA)
        {
            m_pendingSendDataFrames.push_back(txData);
        }
        emit dataSend(txData);
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
            break;
        }
    }
    return deletedPending;
}

bool NetworkNode::addNode(NetworkNode *node)
{
    bool success = false;
    if(node && (m_layer_id == node->getNodeLayer()))
    {
        if(m_connectedNodes.indexOf(node) < 0)
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

void NetworkNode::setNodeRange(quint16 range)
{
    m_range = range;
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

QPoint NetworkNode::getNodePosition() const
{
    return m_node_position;
}

quint16 NetworkNode::getNodeRange() const
{
    return m_range;
}

quint16 NetworkNode::getNumOfConnectedNodes() const
{
    return m_connectedNodes.size();
}

QVector<QPair<quint16, quint16> > NetworkNode::getNeighbours() const
{
    QVector<QPair<quint16, quint16> > neighbours;
    for(NetworkNode *node : m_connectedNodes)
    {
        auto neighbour = qMakePair(node->getNodeID(), getDistanceFromConnectedNode(node->getNodeID()));
        neighbours.push_back(neighbour);
    }
    return neighbours;
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
                        if(checkIfInRange(node->getNodePosition()) && node->checkIfInRange(m_node_position))
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
                disconnected &= node->removeConnectedNode(this);
            }
        }
    }
    return disconnected;
}

bool NetworkNode::removeConnectedNode(NetworkNode *node)
{
    bool removedNode = false;
    if(checkIfConnectedToNode(node))
    {
        if(!node->checkIfConnectedToNode(this))
        {
            m_connectedNodes.remove(m_connectedNodes.indexOf(node));
            removedNode = true;
        }
    }
    return removedNode;
}

bool NetworkNode::disconnectFromNetwork()
{
    bool disconnected = false;
    if(!m_connectedNodes.isEmpty())
    {
        while(!m_connectedNodes.isEmpty())
        {
            auto node = m_connectedNodes[0];
            disconnected &= node->disconnectFromNode(this);
        }
    }
    else
    {
        disconnected = true;
    }
    return disconnected;
}

NetworkNode::NodeType NetworkNode::getNodeType() const
{
    return NodeType::NoType;
}

bool NetworkNode::checkIfInRange(const QPoint &position) const
{
    return getDistanceFromNode(position) <= static_cast<double>(m_range);
}

double NetworkNode::getDistanceFromNode(const QPoint &position) const
{
    double node_x = static_cast<double>(position.x());
    double node_y = static_cast<double>(position.y());
    double x = static_cast<double>(m_node_position.x());
    double y = static_cast<double>(m_node_position.y());
    double result = pow((pow(abs(node_x - x), 2) + pow(abs(node_y - y), 2)), 0.5);
    return result;
}

double NetworkNode::getDistanceFromConnectedNode(double node_id) const
{
    double distance = 0;
    for(NetworkNode* node : m_connectedNodes)
    {
        if(node->getNodeID() == node_id)
        {
            distance = getDistanceFromNode(node->getNodePosition());
        }
    }
    return distance;
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
        case DataFrame::RxData::RECEIVED_DATA : //receiver notification that last send data was accepted
            processReceiveAcknowledged(rxData);
            break;
        case DataFrame::RxData::NO_DATA:
            break;
        default:
            processNewData(rxData);
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
           node.second == connected_node->getNodeLayer())
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


