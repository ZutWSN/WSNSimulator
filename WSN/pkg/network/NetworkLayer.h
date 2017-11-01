#ifndef NETWORKLAYER_H
#define NETWORKLAYER_H
#include <QVector>
#include "NetworkNode.h"

class NetworkLayer
{
public:
    NetworkLayer(qint16 layer_id);
    ~NetworkLayer();
    NetworkLayer(const NetworkLayer &other);
    NetworkLayer(NetworkLayer &&other);
    NetworkLayer& operator=(const NetworkLayer &other);
    NetworkLayer& operator=(NetworkLayer &&other);

    NetworkNode* createNode(NetworkNode::NodeType nodeType, quint16 node_id);
    bool removeNode(NetworkNode *node);

    void setLayerId(quint16);
    qint16 getLayerId() const;
    NetworkNode* getNetworkNode(quint16 node_id) const;
private:
    bool checkIfHasNode(quint16 node_id) const;
    NetworkNode* copyNetworkNode(const NetworkNode *node) const;
private:
    QVector<NetworkNode*> m_nodes;
    qint16 m_layer_id;

};

#endif // NETWORKLAYER_H
