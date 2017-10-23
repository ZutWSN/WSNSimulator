#ifndef NETWORKNODE_H
#define NETWORKNODE_H
#include "DataFrame.h"
#include <QObject>

class NetworkNode : public QObject
{
    Q_OBJECT
public:
    NetworkNode(quint16 node_id = 0, quint16 layer_id = 0);

    bool sendData(const DataFrame &txData);   
    bool connectToNode(NetworkNode *node);

    bool setNodeID(quint16 node_id);
    void setLayer(quint16 layer_id);

    quint16 getNodeID() const;
    quint16 getNodeLayer() const;
    bool getSendDataReceived() const;
public slots:
    void onReceivedData(const DataFrame &rxData);
signals:
    void dataSend(const DataFrame &txData);
private:
    void processData(const DataFrame &rxData);
private:
    bool m_sendDataReceived;
    quint16 m_node_id;
    quint16 m_layer_id;
};

#endif // NETWORKNODE_H
