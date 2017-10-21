#include "NetworkNode.h"

NetworkNode::NetworkNode()
{

}

bool NetworkNode::sendData(const QByteArray &data) const
{
    bool result = false;
    if(data.contains("Data"))
    {
        result = true;
    }
    return result;
}

