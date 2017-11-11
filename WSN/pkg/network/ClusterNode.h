#ifndef CLUSTERNODE_H
#define CLUSTERNODE_H
#include "NetworkNode.h"
#include <QVector>

class ClusterNode : public NetworkNode
{
    Q_OBJECT
public:
    ClusterNode(quint16 node_id);
    ~ClusterNode();

    bool broadCastDataToSensors() const;

    NetworkNode::NodeType getNodeType() const;
public slots:
    onReceivedData(const DataFrame &rxData);
private:
    void processData(const DataFrame &rxData);
};

#endif // CLUSTERNODE_H
