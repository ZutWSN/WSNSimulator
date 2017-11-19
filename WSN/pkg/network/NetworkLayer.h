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

    bool createNode(NetworkNode::NodeType nodeType, quint16 node_id);
    bool connectNodes(quint16 first_node, quint16 second_node);
    bool connectNodeWidget(quint16 node_id, QWidget* widget);
    bool disconnectNodeWidget(quint16 node_id);
    bool removeNode(quint16 node_id);

    void setLayerId(quint16 id);
    qint16 getLayerId() const;
    quint16 getNumOfNodes() const;
    //think about getter for node pointer, for network manager,make it private
    //so that only friend class can use it.
private:
    qint16 checkIfHasNode(quint16 node_id) const;
    NetworkNode* copyNetworkNode(const NetworkNode *node) const;
private:
    QVector<NetworkNode*> m_nodes;
    qint16 m_layer_id;

};

#endif // NETWORKLAYER_H
