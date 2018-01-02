#include "DataFrame.h"
#include <utility>

const QVector<QString> widgetStateNames =
{
    "NO_DATA",
    "NEW_DATA",
    "RECEIVED_DATA",
    "PATH_SYNC",
    "SENSOR_BROADCAST",
    "NEIGHBOUR_PATH_REQ",
    "NEIGHBOUR_PATH",
    "CLUSTER_PATH",
    "SENSOR_RESPONSE",
    "REMOVED_NODE",
    "DIRECT_CLUSTER_REMOVED",
    "NO_SINK_CONNECTION"
};

DataFrame::DataFrame():
    m_Msg(QByteArray()),
    m_Type(RxData::NO_DATA),
    m_sender(QPair<quint16, quint16>(0xFF, 0xFF)),
    m_desination(QPair<quint16, quint16>(0xFF, 0xFF))
{

}

DataFrame::DataFrame(const QByteArray &msg,
                     DataFrame::RxData type,
                     quint16 destination_id,
                     quint16 layer_id,
                     quint16 sender_id):
    m_Msg(msg),
    m_Type(type),
    m_sender(qMakePair(sender_id, layer_id)),
    m_desination(qMakePair(destination_id, layer_id))
{

}

DataFrame::DataFrame(const DataFrame &other)
{
    if(this != &other)
    {
        m_Msg = other.m_Msg;
        m_Type = other.m_Type;
        m_sender = other.m_sender;
        m_desination = other.m_desination;
        m_path = other.m_path;
        m_visitedNodes = other.m_visitedNodes;
    }
}

DataFrame::DataFrame(DataFrame &&other)
{
    if(this != &other)
    {
        m_Msg = other.m_Msg;
        m_Type = other.m_Type;
        m_sender = std::move(other.m_sender);
        other.m_Msg = std::move(other.m_Msg);
        m_visitedNodes = std::move(other.m_visitedNodes);
        other.m_Type = other.m_Type;
    }
}

DataFrame& DataFrame::operator=(const DataFrame &other)
{
    if(this != &other)
    {
        m_Msg = other.m_Msg;
        m_Type = other.m_Type;
        m_sender = other.m_sender;
        m_desination = other.m_desination;
        m_path = other.m_path;
        m_visitedNodes = other.m_visitedNodes;
    }
    return *this;
}

DataFrame& DataFrame::operator=(DataFrame &&other)
{
    if(this != &other)
    {
        m_Msg = other.m_Msg;
        m_Type = other.m_Type;
        m_sender = std::move(other.m_sender);
        other.m_Msg = std::move(other.m_Msg);
        m_visitedNodes = std::move(other.m_visitedNodes);
        other.m_Type = other.m_Type;
    }
    return *this;
}

DataFrame::~DataFrame()
{

}

void DataFrame::setMsg(const QByteArray &msg)
{
   m_Msg = msg;
}

void DataFrame::setMsgType(DataFrame::RxData type)
{
    m_Type = type;
}

void DataFrame::setSender(const QPair<quint16, quint16> &sender)
{
    m_sender = sender;
}

bool DataFrame::addVisitedNode(const QPair<quint16, quint16> &node)
{
    bool visited = false;
    if(!hasVisitedNode(node))
    {
        m_visitedNodes.push_back(node);
        visited = true;
    }
    return visited;
}

bool DataFrame::setPath(const QVector<quint16> &path)
{
    bool pathSet = false;
    if(!path.isEmpty())
    {
        m_path = path;
        pathSet = true;
    }
    return pathSet;
}

void DataFrame::setDestination(const QPair<quint16, quint16> &destination)
{
    m_desination = destination;
}

QByteArray DataFrame::getMsg() const
{
    return m_Msg;
}

DataFrame::RxData DataFrame::getMsgType() const
{
    return m_Type;
}

QString DataFrame::getMsgTypeString() const
{
    return widgetStateNames[m_Type];
}

QPair<quint16, quint16> DataFrame::getSender() const
{
    return m_sender;
}

QVector<quint16> DataFrame::getPath() const
{
    return m_path;
}

QByteArray DataFrame::getMsgInfo() const
{
    QByteArray log;
    log.append("SENDER ID: " + QString::number(m_sender.first) + "\n");
    log.append("SENDER LAYER: " + QString::number(m_sender.second) + "\n");
    log.append("MESSAGE TYPE: " + getMsgTypeString() + "\n");
    log.append("MESSAGE PATH: ");
    for(auto && id : m_path)
    {
        log.append(QString::number(id) +  " ");
    }
    log.append("Sink\n");
    return log;
}

QPair<quint16, quint16> DataFrame::getNextChainNode(quint16 currentNodeID, quint16 currentNodeLayer) const
{
    QPair<quint16, quint16> nextNode;
    if(m_desination.first == currentNodeID && m_desination.second == currentNodeLayer)
    {
        for(quint16 i = 0; i < m_path.size(); i++)
        {
            if(currentNodeID == m_path[i])
            {
                nextNode = qMakePair(m_path[i + 1], currentNodeLayer);
            }
        }
    }
    return nextNode;
}

bool DataFrame::hasVisitedNode(const QPair<quint16, quint16> &node) const
{
    return (m_visitedNodes.indexOf(node) >= 0);
}

QPair<quint16, quint16> DataFrame::getDestination() const
{
    return m_desination;
}

bool DataFrame::frameEmpty() const
{
    return m_Msg.isEmpty();
}

bool DataFrame::isFinalDestination(const QPair<quint16, quint16> &nodeLayerAndID) const
{
    bool finalDest = false;
    if(nodeLayerAndID.second == m_sender.second)
    {
        finalDest = (m_path.indexOf(nodeLayerAndID.first) == (m_path.size() - 1));
    }
    return finalDest;
}

