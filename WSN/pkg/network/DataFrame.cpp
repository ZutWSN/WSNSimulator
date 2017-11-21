#include "DataFrame.h"
#include <utility>
#include <QPair>
#include <QVector>

DataFrame::DataFrame():
    m_Msg(QByteArray()),
    m_Type(RxData::NO_DATA),
    m_desination_id(0)
{

}

DataFrame::DataFrame(const QByteArray &msg,
                     DataFrame::RxData type,
                     quint16 destination_id,
                     quint16 layer_id, quint16 sender_id, quint16 sender_id):
    m_Msg(msg),
    m_Type(type),
    m_desination(qMakePair(destination_id, layer_id))
{

}

DataFrame::DataFrame(const DataFrame &other)
{
    if(this != &other)
    {
        m_Msg = other.m_Msg;
        m_Type = other.m_Type;
        m_desination_id = other.m_desination_id;
    }
}

DataFrame::DataFrame(DataFrame &&other)
{
    if(this != &other)
    {
        m_Msg = other.m_Msg;
        m_Type = other.m_Type;
        m_desination_id = other.m_desination_id;
        other.m_Msg = QByteArray();
        other.m_desination_id = 0;
        other.m_Type = RxData::NO_DATA;
    }
}

DataFrame& DataFrame::operator=(const DataFrame &other)
{
    if(this != &other)
    {
        m_Msg = other.m_Msg;
        m_Type = other.m_Type;
        m_desination_id = other.m_desination_id;
    }
    return *this;
}

DataFrame& DataFrame::operator=(DataFrame &&other)
{
    if(this != &other)
    {
        m_Msg = other.m_Msg;
        m_Type = other.m_Type;
        m_desination_id = other.m_desination_id;
        other.m_Msg = QByteArray();
        other.m_desination_id = 0;
        other.m_Type = RxData::NO_DATA;
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

bool DataFrame::setPath(const QVector<QPair<quint16, quint16> > &path)
{
    bool pathSet = false;
    if(path.isEmpty())
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

QPair<quint16, quint16> DataFrame::getSender() const
{
    return m_sender;
}

QPair<quint16, quint16> DataFrame::getNextChainNode(quint16 currentNodeID) const
{
    QPair<quint16, quint16> nextNode;
    for(quint16 i = 0; i < m_path.size(); i++)
    {
        if(currentNodeID == m_path[i].first)
        {
            nextNode = m_path[i + 1];
        }
    }
    return nextNode;
}

QPair<quint16, quint16> DataFrame::getDestination() const
{
    return m_desination;
}

bool DataFrame::frameEmpty() const
{
    return m_Msg.isEmpty();
}


