#include "sinknode.h"

SinkNode *SinkNode::getSinkInstance(const QPoint &node_position, quint16 range)
{
    if(!m_sinkInstance)
    {
        m_sinkInstance = new SinkNode;
        m_sinkInstance->setPosition(node_position);
        m_sinkInstance->setRange(range);
    }
    return m_sinkInstance;
}

bool SinkNode::addDirectCluster(NetworkNode *cluster)
{

}

bool SinkNode::removeDirectCluster(NetworkNode *cluster)
{

}

void SinkNode::setPosition(const QPoint &pos)
{

}

void SinkNode::setRange(const QPoint &range)
{

}

void SinkNode::getNumOfInRangeCusters() const
{

}

bool SinkNode::addCluster(NetworkNode *cluster)
{

}

void SinkNode::getNumOfConnectedLayers() const
{

}

void SinkNode::onReceivedDataFromCluster(const DataFrame &data)
{

}

void SinkNode::processNewData(const DataFrame &data)
{

}

