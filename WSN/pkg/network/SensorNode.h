#ifndef SENSORNODE_H
#define SENSORNODE_H
#include "NetworkNode.h"

class SensorNode : public NetworkNode
{
public:
    SensorNode(quint16 node_id);

    NetworkNode::NodeType getNodeType() const;
};

#endif // SENSORNODE_H
