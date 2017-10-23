#ifndef NETWORKLAYER_H
#define NETWORKLAYER_H
#include <QVector>
#include "NetworkNode.h"

class NetworkLayer
{
public:
    NetworkLayer();
    ~NetworkLayer();

    bool addNode(NetworkNode *node);
    bool removeNode(NetworkNode *node);
    qint16 checkIfHasNode(quint16 node_id) const;

    void setLayerId(quint16);
    quint16 getLayerId() const;
private:
    QVector<NetworkNode*> m_nodes;
    quint16 m_layer_id;
};

#endif // NETWORKLAYER_H
