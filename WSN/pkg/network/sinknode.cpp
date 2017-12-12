#include "sinknode.h"
#include "ClusterNode.h"
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
    if(cluster && (cluster->getNodeType() == NetworkNode::NodeType::Cluster))
    {
        if(cluster->checkIfInRange(m_position))
        {
            addedCluster = static_cast<bool>(connect(this, SIGNAL(broadCastDataToClusters(DataFrame)), node, SLOT(onReceivedData(DataFrame))));
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

bool SinkNode::checkIfHasDirectCluster(NetworkNode *cluster) const
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
        case DataFrame::RxData::REMOVED_NODE:
            QByteArray msg;
            if(removeNode(msg))
            {
                if(!m_inRangeClusters.isEmpty())
                {
                    if(calculateNetworkPaths(data, msg))
                    {
                        DataFrame pathUpdate(msg, DataFrame::RxData::PATH_SYNC);
                        emit broadCastDataToClusters(pathUpdate);
                    }
                }
                else    //node removed was last in range cluster
                {
                    //do nothing until some in range cluster is added
                    //to forward network data. After that send path update
                    //message to connect rest of the network again -
                    //once more calculate networkPaths and update local memebers.
                }
            }
            break;
        default:
            break;
    }
}

bool SinkNode::calculateNetworkPaths(const DataFrame &data, QByteArray &updateMsg)
{
    bool createdUpdateMsg = false;
    QVector<Vertice> graph = createGraph();
    return createdUpdateMsg;
}

bool SinkNode::updateClusterPath(const DataFrame &data)
{
    bool pathUpdated = false;
    QJsonDocument jsonData = QJsonDocument::fromBinaryData(data.getMsg(), QJsonDocument::Validate);
    if(!jsonData.isNull())
    {
        QJsonObject jsonObj = jsonData.object();
        quint16 node_id, layer_id;
        quint8 parameters = 0;
        QPoint position;
        QVector<quint16> nodePath;
        QVector<quint16> neighbourIDs;
        QVector<quint16> neighbourDistances;
        auto keys = jsonObj.keys();
        for(auto && key : keys)
        {
            if(key == ClusterNode::NODE_ID)
            {
                node_id = static_cast<quint16>(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == ClusterNode::LAYER_ID)
            {
                layer_id = static_cast<quint16>(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == ClusterNode::NODE_POSITION_X)
            {
                position.setX(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == ClusterNode::NODE_POSITION_Y)
            {
                position.setY(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == ClusterNode::PATH)
            {
                nodePath.clear();
                for(auto && id: jsonObj[key].toArray())
                {
                    if(id.isDouble())
                    {
                        nodePath.push_back(static_cast<quint16>(id.toInt()));
                    }
                }
                if(!nodePath.isEmpty())
                {
                    ++parameters;
                }
            }
            else if(key == ClusterNode::NEIGHBOURS_IDS)
            {
                neighbourIDs.clear();
                for(auto && id: jsonObj[key].toArray())
                {
                    if(id.isDouble())
                    {
                        neighbourIDs.push_back(static_cast<quint16>(id.toInt()));
                    }
                }
                if(!neighbourIDs.isEmpty())
                {
                    ++parameters;
                }
            }
            else if(key == ClusterNode::NEIGHBOURS_DISTANCES)
            {
                neighbourDistances.clear();
                for(auto && id: jsonObj[key].toArray())
                {
                    if(id.isDouble())
                    {
                        neighbourDistances.push_back(static_cast<quint16>(id.toInt()));
                    }
                }
                if(!neighbourDistances.isEmpty())
                {
                    ++parameters;
                }
            }
            else if(key == ClusterNode::PATH_LENGTH)
            {
                pathLength = static_cast<quint16>(jsonObj[key].toInt());
                if(pathLength > 0)
                {
                    ++parameters;
                }
            }
        }
        if(parameters == ClusterNode::NUM_OF_CLUSTER_MESSAGE_PARAMS)
        {
            if(neighbourDistances.size() == neighbourIDs.size())
            {
                MappedClusterNode node{ node_id,
                                        layer_id,
                                        position,
                                        pathLength,
                                        neighbourIDs,
                                        neighbourDistances,
                                        nodePath};
                qint16 index = checkIfHasMappedCluster(node);
                if(index >= 0)
                {
                    //update the node path
                    m_clusterPathMap[index] = node;
                }
                else
                {
                    //map new node
                    m_clusterPathMap.push_back(node);
                }
                pathUpdated = true;
            }
        }
    }
}

quint16 SinkNode::checkIfHasMappedCluster(const MappedClusterNode &node) const
{
    quint16 index = -1;
    quint16 i = 0;
    for(auto && mappedNode : m_clusterPathMap)
    {
        if(mappedNode == node)
        {
            index = i;
            break;
        }
        ++i;
    }
    return index;
}

bool SinkNode::removeNode(const QByteArray &msg)
{
    //extract from message which node was removed
    //and remove it either from direct clusters or from
    //mapped nodes
    bool nodeRemoved = true;
    QJsonDocument jsonData = QJsonDocument::fromBinaryData(data.getMsg(), QJsonDocument::Validate);
    if(!jsonData.isNull())
    {
        QJsonObject jsonObj = jsonData.object();
        quint16 node_id, layer_id;
        quint8 parameters = 0;
        QPoint position;
        ClusterNode::ClusterStates node_state;
        auto keys = jsonObj.keys();
        for(auto && key : keys)
        {
            if(key == ClusterNode::NODE_ID)
            {
                node_id = static_cast<quint16>(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == ClusterNode::LAYER_ID)
            {
                layer_id = static_cast<quint16>(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == ClusterNode::NODE_POSITION_X)
            {
                position.setX(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == ClusterNode::NODE_POSITION_Y)
            {
                position.setY(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == ClusterNode::NODE_STATE)
            {
                node_state = static_cast<ClusterNode::ClusterStates>(jsonObj[key].toInt());
                ++parameters;
            }
        }
        if(parameters == ClusterNode::NUM_OF_REMOVE_MESSAGE_PARAMS)
        {
            if(node_state == ClusterNode::ClusterStates::CONNECTED_TO_SINK)
            {
                ClusterNode clusterToRemove(node_id, 0, layer_id, position);
                quint16 i = 0;
                for(NetworkNode *directCluster : m_inRangeClusters)
                {
                    ClusterNode *cluster = static_cast<ClusterNode*>(directCluster);
                    if(*cluster == clusterToRemove)
                    {
                        //remove this cluster from in range cluster array
                        //and all mapped nodes neighbours.
                        for(auto && neighbourNode : cluster->getNeighbours())
                        {
                            for(MappedClusterNode &mappedNode : m_clusterPathMap)
                            {
                                if(mappedNode.layer_id == cluster->getNodeLayer())
                                {
                                    if(neighbourNode.first == mappedNode.node_id)
                                    {
                                        int idx = mappedNode.neighbourIDs.indexOf(cluster->getNodeID());
                                        if(idx >= 0)
                                        {
                                            mappedNode.neighbourIDs.remove(idx);
                                            mappedNode.neighbourDistances.remove(idx);
                                        }
                                        else
                                        {
                                            nodeRemoved = false;
                                        }
                                    }
                                }
                            }
                        }
                        m_inRangeClusters.remove(m_inRangeClusters.indexOf(directCluster));
                    }
                    ++i;
                }
            }
            else
            {
                MappedClusterNode node;
                node.node_id = node_id;
                node.layer_id = layer_id;
                int idx = m_clusterPathMap.indexOf(node);
                if(idx >= 0)
                {
                    for(auto && neighbourID: m_clusterPathMap[idx].neighbourIDs)
                    {
                        for(MappedClusterNode &mappedNode : m_clusterPathMap)
                        {
                            if(mappedNode.layer_id == layer_id)
                            {
                                if(neighbourNode.first == node_id)
                                {
                                    int index = mappedNode.neighbourIDs.indexOf(node_id);
                                    if(index >= 0)
                                    {
                                        mappedNode.neighbourIDs.remove(index);
                                        mappedNode.neighbourDistances.remove(index);
                                    }
                                    else
                                    {
                                        nodeRemoved = false;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    nodeRemoved = false;
                }

            }

        }
        else
        {
            nodeRemoved = false;
        }
    }
    return nodeRemoved;
}

void SinkNode::extractNodeData(NetworkNode *node, SinkNode::Vertice &vertice) const
{
    vertice.node_id = node->getNodeID();
    for(auto && neighbour : node->getNeighbours())
    {
        vertice.neighbours.push_back(neighbour);
    }
}

void SinkNode::extractMappedNodeData(const MappedClusterNode &node, SinkNode::Vertice &vertice) const
{
    vertice.node_id = node.node_id;
    for(quint16 i = 0; i < node.neighbourIDs.size(); i++)
    {
        vertice.neighbours.push_back(qMakePair(node.neighbourIDs[i], node.neighbourDistances[i]));
    }
}

QVector<SinkNode::Vertice> SinkNode::createGraph() const
{
    QVector<Vertice> vertices;
    Vertice vertice;
    for(NetworkNode* directNode : m_inRangeClusters)
    {
        extractNodeData(directNode, vertice);
        vertices.push_back(vertice);
    }
    for(auto && mappedNode : m_clusterPathMap)
    {
        extractMappedNodeData(mappedNode, vertice);
        vertices.push_back(vertice);
    }
    for(auto && v : vertices)
    {
        for(auto && idAndDist : v.neighbours)
        {
            vertice.node_id = idAndDist.first;
            int idx = vertices.indexOf(vertice);
            v.neighbourVerticesIndexes.push_back(idx);
            v.neighbourVerticesDistances(idAndDist.second);
        }
    }
    return vertices;
}


