#ifndef NETWORKNODE_H
#define NETWORKNODE_H
#include "DataFrame.h"
#include <QObject>

class NetworkNode : public QObject
{
    Q_OBJECT
public:
    NetworkNode();
    bool sendData(const DataFrame &txData);
public slots:
    void onReceivedData(const DataFrame &rxData);
signals:
    void dataSend(const DataFrame &txData);
private:
    bool m_sendDataReceived;
};

#endif // NETWORKNODE_H
