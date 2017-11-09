#ifndef SENSORNODE_H
#define SENSORNODE_H
#include "NetworkNode.h"

class ClusterNode;

class SensorNode : public NetworkNode
{
public:
    SensorNode(quint16 node_id);
    bool disconnectFromCluster();
    NetworkNode::NodeType getNodeType() const;
    bool isConnectedToCluster() const;
private:
    bool connectToCluster(ClusterNode *cluster);
private:
    quint16 m_cluster_id;
    bool m_connectedToCluster;
};

#endif // SENSORNODE_H
