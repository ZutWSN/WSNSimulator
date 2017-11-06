#include "DataFrame.h"
#include <utility>

DataFrame::DataFrame():
    m_Msg(QByteArray()),
    m_Type(RxData::NO_DATA),
    m_desination_id(0)
{

}

DataFrame::DataFrame(const QByteArray &msg, DataFrame::RxData type, quint16 destination_id):
    m_Msg(msg),
    m_Type(type),
    m_desination_id(destination_id)
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

void DataFrame::setDestination(quint16 node_id)
{
    m_desination_id = node_id;
}

QByteArray DataFrame::getMsg() const
{
    return m_Msg;
}

DataFrame::RxData DataFrame::getMsgType() const
{
    return m_Type;
}

quint16 DataFrame::getDestination() const
{
    return m_desination_id;
}

bool DataFrame::frameEmpty() const
{
    return m_Msg.isEmpty();
}


