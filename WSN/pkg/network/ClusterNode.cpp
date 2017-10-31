#include "ClusterNode.h"

ClusterNode::ClusterNode(quint16 node_id) :
    NetworkNode(node_id)
{

}

NetworkNode::NodeType ClusterNode::getNodeType() const
{
    return NodeType::Cluster;
}

