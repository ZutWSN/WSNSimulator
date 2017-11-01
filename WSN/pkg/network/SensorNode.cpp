#include "SensorNode.h"

SensorNode::SensorNode(quint16 node_id):
    NetworkNode(node_id)
{

}

NetworkNode::NodeType SensorNode::getNodeType() const
{
    return NodeType::Sensor;
}
