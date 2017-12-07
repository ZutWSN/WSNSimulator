#include "sinknode.h"
#include "DragWidget.h"

SinkNode *SinkNode::getSinkInstance()
{
    if(!m_sinkInstance)
    {
        m_sinkInstance = new SinkNode;
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
                cluster->setConnectedToSink(this);
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

bool SinkNode::connectToNodeWidget(QWidget *widget)
{
    bool success = false;
    DragWidget *dragWidget = static_cast<DragWidget*>(widget);
    if(dragWidget && !dragWidget->isConnectedToNode())
    {
        if(!m_connectedToWidget)
        {
            success = static_cast<bool>(connect(this, SIGNAL(receivedNewData(DataFrame)), dragWidget, SLOT(onNodeReceivedData(DataFrame))));
            m_connectedToWidget = success;
            if(m_connectedToWidget)
            {
                dragWidget->setConnectedToNode(true);
                m_Widget = widget;
            }
        }
    }
    return success;
}

bool SinkNode::disconnectFromWidget()
{
    bool disconnected = false;
    if(m_connectedToWidget && m_Widget)
    {
        DragWidget *dragWidget = static_cast<DragWidget*>(m_Widget);
        if(dragWidget->isConnectedToNode())
        {
            disconnected = disconnect(this, 0, m_Widget, 0);
        }
        if(disconnected)
        {
            dragWidget->setConnectedToNode(false);
            m_Widget = nullptr;
            m_connectedToWidget = false;
        }
    }
    return disconnected;
}

void SinkNode::sendNewPaths()
{

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

DataFrame SinkNode::getLastMsg() const
{
    return m_lastMsg;
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
    m_lastMsg = data;
    switch(rxData.getMsgType())
    {
        case DataFrame::RxData::CLUSTER_PATH:
            updateClusterPath(data);
            break;
        case DataFrame::RxData::NEW_DATA:
            emit receivedData(data);
            break;
    }
}

void SinkNode::broadCastDataToClusters(const DataFrame &data)
{

}

void SinkNode::calculateNetworkPaths()
{

}

void SinkNode::updateClusterPath(const DataFrame &data)
{

}


