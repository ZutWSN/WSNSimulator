#ifndef CLUSTERNODE_H
#define CLUSTERNODE_H
#include "NetworkNode.h"
#include <QVector>

class SensorNode;

class ClusterNode : public NetworkNode
{
public:
    ClusterNode(quint16 node_id);

    bool addSensorNode(SensorNode *sensor);
    bool connectToNode(NetworkNode *node)
    bool addNeighbourCluster(ClusterNode* cluster);

    NetworkNode::NodeType getNodeType() const;
private:
    bool checkIfSensorConnected(quint16 sensor_id);
private:
    QVector<SensorNode*> m_sensors;
    QVector<ClusterNode*> m_clusterNeighbours;
};

#endif // CLUSTERNODE_H
