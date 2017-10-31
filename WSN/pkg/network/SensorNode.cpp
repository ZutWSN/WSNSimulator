#include "SensorNode.h"

SensorNode::SensorNode()
{

}

NetworkNode::NodeType SensorNode::getNodeType() const
{
    return NodeType::Sensor;
}
