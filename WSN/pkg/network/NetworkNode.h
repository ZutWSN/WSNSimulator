#ifndef NETWORKNODE_H
#define NETWORKNODE_H
#include <QByteArray>
class NetworkNode
{
public:
    NetworkNode();
    bool sendData(const QByteArray &data) const;
};

#endif // NETWORKNODE_H
