#ifndef SENSORNODE_H
#define SENSORNODE_H
#include "NetworkNode.h"

class SensorNode : public NetworkNode
{
    Q_OBJECT
public:
    SensorNode(quint16 node_id);
    bool disconnectFromNode(NetworkNode *node);
    bool connectToNode(NetworkNode *node);
    NetworkNode::NodeType getNodeType() const;
    bool isConnectedToCluster() const;
private:
    void processData(const DataFrame &rxData);
private:
    quint16 m_cluster_id;
    bool m_connectedToCluster;
};

#endif // SENSORNODE_H
