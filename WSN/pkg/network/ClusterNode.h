#ifndef CLUSTERNODE_H
#define CLUSTERNODE_H
#include "NetworkNode.h"
#include <QString>

class ClusterNode : public NetworkNode
{
    Q_OBJECT
public:
    ClusterNode(quint16 node_id);
    ClusterNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position);
    ~ClusterNode();

    NetworkNode::NodeType getNodeType() const;
signals:
    void broacastDataToSensors(const DataFrame &rxData);
private:
    void processNewData(const DataFrame &rxData);
private:
    QString pathToSink;
};

#endif // CLUSTERNODE_H
