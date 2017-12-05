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
    bool addedCluster = false;
    if(cluster)
    {
        if(cluster->checkIfInRange(m_position))
        {
            addedCluster = static_cast<bool>(connect(this, SIGNAL(sendData(DataFrame)), node, SLOT(onReceivedData(DataFrame))));
            addedCluster &= static_cast<bool>(connect(node, SIGNAL(dataSend(DataFrame)), this, SLOT(onReceivedDataFromCluster(DataFrame))));
            if(addedCluster)
            {
                m_inRangeClusters.push_back(cluster);
            }
        }
    }
    return addedCluster;
}

bool SinkNode::removeDirectCluster(NetworkNode *cluster)
{
    bool removed = false;
    if(cluster)
    {
        if(checkIfHasCluster(cluster))
        {
            removed = disconnect(this, 0, node, 0);
            removed &= disconnect(node, 0, this, 0);
            if(removed)
            {
                m_inRangeClusters.remove(m_inRangeClusters.indexOf(node));
            }
        }
    }
    return removed;
}

void SinkNode::setPosition(const QPoint &pos)
{
    m_position = pos;
}

void SinkNode::setRange(const QPoint &range)
{
    m_range = range;
}

void SinkNode::getNumOfInRangeCusters() const
{
    return m_inRangeClusters.size();
}

void SinkNode::getNumOfConnectedLayers() const
{
    return m_layers.size();
}

bool SinkNode::checkIfHasCluster(NetworkNode *cluster) const
{
    bool present = false;
    for(NetworkNode* node : m_inRangeClusters)
    {
        if(node == cluster)
        {
            present = true;
            break;
        }
    }
    return present;
}

void SinkNode::onReceivedDataFromCluster(const DataFrame &data)
{
    switch(rxData.getMsgType())
    {
        case DataFrame::RxData::CLUSTER_PATH:
            updateClusterPath(data);
            break;
        case DataFrame::RxData::NEW_DATA:
            break;
    }
}

void SinkNode::processNewData(const DataFrame &data)
{

}

