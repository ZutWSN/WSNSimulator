#ifndef SENSORNODE_H
#define SENSORNODE_H
#include "NetworkNode.h"

class SensorNode : public NetworkNode
{
public:
    SensorNode();

    NetworkNode::NodeType getNodeType() const;
};

#endif // SENSORNODE_H
