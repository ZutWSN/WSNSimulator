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

QByteArray DataFrame::getMsg() const
{
    return m_Msg;
}

DataFrame::RxData DataFrame::getMsgType() const
{
    return m_Type;
}

bool DataFrame::frameEmpty() const
{
    return m_Msg.isEmpty();
}


