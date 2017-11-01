#ifndef CLUSTERNODE_H
#define CLUSTERNODE_H
#include "NetworkNode.h"

class ClusterNode : public NetworkNode
{
public:
    ClusterNode(quint16 node_id);
    NetworkNode::NodeType getNodeType() const;
};

#endif // CLUSTERNODE_H
