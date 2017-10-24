#ifndef NETWORKLAYER_H
#define NETWORKLAYER_H
#include <QVector>
#include "NetworkNode.h"

class NetworkLayer
{
public:
    NetworkLayer(qint16 layer_id = -1);
    ~NetworkLayer();

    bool addNode(NetworkNode *node);
    bool removeNode(NetworkNode *node);

    void setLayerId(quint16);
    qint16 getLayerId() const;
private:
    qint16 checkIfHasNode(quint16 node_id) const;
private:
    QVector<NetworkNode*> m_nodes;
    qint16 m_layer_id;
};

#endif // NETWORKLAYER_H
