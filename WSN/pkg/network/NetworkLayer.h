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
    bool connectNodes(quint16 first_node, quint16 second_node);
    bool connectNodeWidget(quint16 node_id, QWidget* widget);
    bool disconnectNodeWidget(quint16 node_id);
    bool removeNode(quint16 node_id);

    void setLayerId(quint16 id);
    qint16 getLayerId() const;
    quint16 getNumOfNodes() const;
    NetworkNode *getNode(quint16 id) const;
    bool checkIfIdAvailable(quint16 id) const;
    QVector<NetworkNode*>::const_iterator getIteratorToFirstNode();
    //make sensor network a friend so it can access layer nodes via private getter or directly from vector m_nodes
private:
    qint16 checkIfHasNode(quint16 node_id) const;
    NetworkNode* copyNetworkNode(const NetworkNode *node) const;
    QByteArray createNodeRemovalMsg(quint16 node_id) const;
    void reassignSensorNodes(quint16 node_id);
private:
    QVector<NetworkNode*> m_nodes;
    static QVector<quint16> m_usedIDPool;
    qint16 m_layer_id;

};

#endif // NETWORKLAYER_H
