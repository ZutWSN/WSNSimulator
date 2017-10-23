#include "DataFrame.h"

DataFrame::DataFrame()
{

}

DataFrame::DataFrame(const DataFrame &other)
{
    m_Msg = other.m_Msg;
    m_Type = other.m_Type;
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


