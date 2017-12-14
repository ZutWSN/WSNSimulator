#include "sinknode.h"
#include "ClusterNode.h"
#include "DragWidget.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>

const quint16 SINK_ID = UINT16_MAX;

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
        ClusterNode* clusterNode = static_cast<ClusterNode*>(cluster);
        if(cluster->checkIfInRange(m_position))
        {
            addedCluster = static_cast<bool>(connect(this, SIGNAL(broadCastDataToClusters(DataFrame)), clusterNode, SLOT(onReceivedData(DataFrame))));
            addedCluster &= static_cast<bool>(connect(clusterNode, SIGNAL(dataSend(DataFrame)), this, SLOT(onReceivedDataFromCluster(DataFrame))));
            if(addedCluster)
            {
                m_inRangeClusters.push_back(cluster);
                clusterNode->setConnectedToSink(this);
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
        if(checkIfHasDirectCluster(cluster))
        {
            removed = disconnect(this, 0, cluster, 0);
            removed &= disconnect(cluster, 0, this, 0);
            if(removed)
            {
                m_inRangeClusters.remove(m_inRangeClusters.indexOf(cluster));
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

void SinkNode::setRange(quint16 &range)
{
    m_range = range;
}

quint16 SinkNode::getNumOfInRangeCusters() const
{
    return m_inRangeClusters.size();
}

quint16 SinkNode::getNumOfConnectedLayers() const
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
    switch(data.getMsgType())
    {
        case DataFrame::RxData::CLUSTER_PATH:
            updateClusterPath(data);
            break;
        case DataFrame::RxData::NEW_DATA:
            emit receivedData(data);
            break;
        case DataFrame::RxData::REMOVED_NODE:
            if(removeNode(data.getMsg()))
            {
                if(!m_inRangeClusters.isEmpty())
                {
                    QByteArray msg;
                    if(calculateNetworkPaths(msg))
                    {
                        DataFrame pathUpdate(msg, DataFrame::RxData::PATH_SYNC, 0, 0, 0);
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

bool SinkNode::calculateNetworkPaths(QByteArray &updateMsg)
{
    bool createdUpdateMsg = false;
    QVector<Vertice> vertices = createGraphAndFindPaths();
    //update the paths
    createdUpdateMsg = updatePathsAndCreateSyncMsg(vertices, updateMsg);
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
        quint16 pathLength = 0;
        QPoint position;
        QVector<quint16> nodePath;
        QVector<quint16> neighbourIDs;
        QVector<quint16> neighbourDistances;
        auto keys = jsonObj.keys();
        for(auto && key : keys)
        {
            if(key == NODE_ID)
            {
                node_id = static_cast<quint16>(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == LAYER_ID)
            {
                layer_id = static_cast<quint16>(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == NODE_POSITION_X)
            {
                position.setX(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == NODE_POSITION_Y)
            {
                position.setY(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == PATH)
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
            else if(key == NEIGHBOURS_IDS)
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
            else if(key == NEIGHBOURS_DISTANCES)
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
            else if(key == PATH_LENGTH)
            {
                pathLength = static_cast<quint16>(jsonObj[key].toInt());
                if(pathLength > 0)
                {
                    ++parameters;
                }
            }
        }
        if(parameters == NUM_OF_PATH_MESSAGE_PARAMS)
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
    return pathUpdated;
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
    QJsonDocument jsonData = QJsonDocument::fromBinaryData(msg.getMsg(), QJsonDocument::Validate);
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
            if(key == NODE_ID)
            {
                node_id = static_cast<quint16>(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == LAYER_ID)
            {
                layer_id = static_cast<quint16>(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == NODE_POSITION_X)
            {
                position.setX(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == NODE_POSITION_Y)
            {
                position.setY(jsonObj[key].toInt());
                ++parameters;
            }
            else if(key == NODE_STATE)
            {
                node_state = static_cast<ClusterNode::ClusterStates>(jsonObj[key].toInt());
                ++parameters;
            }
        }
        if(parameters == NUM_OF_REMOVE_MESSAGE_PARAMS)
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
    vertice.layer_id = directNode->getNodeID();
    for(auto && neighbour : node->getNeighbours())
    {
        vertice.neighbours.push_back(neighbour);
    }
}

void SinkNode::extractMappedNodeData(const MappedClusterNode &node, SinkNode::Vertice &vertice) const
{
    vertice.node_id = node.node_id;
    vertice.layer_id = node.layer_id;
    for(quint16 i = 0; i < node.neighbourIDs.size(); i++)
    {
        vertice.neighbours.push_back(qMakePair(node.neighbourIDs[i], node.neighbourDistances[i]));
    }
}

QVector<SinkNode::Vertice> SinkNode::createGraphAndFindPaths() const
{
    QVector<Vertice> vertices;
    Vertice vertice, sinkVertice;
    const quint16 sinkIndex = 0;
    quint16 index = 0;
    vertice.sinkPathLength = UINT16_MAX;
    vertice.isDirectVertex = true;
    //create sink start vertice and put its neighbours in vertices vector
    for(NetworkNode* directNode : m_inRangeClusters)
    {
        //the sink will be inserted at the beginning of vertices vector
        //so all of its neighbour vertices will be increased by one
        sinkVertice.neighbourVerticesIndexes = index + 1;
        sinkVertice.neighbourVerticesDistances.push_back(directNode->getDistanceFromNode(m_position));
        //add sink vertice to direct cluster vertices
        vertice.neighbourVerticesIndexes.push_back(sinkIndex);
        vertice.neighbourVerticesDistances.push_back(directNode->getDistanceFromNode(m_position));
        extractNodeData(directNode, vertice);

        vertices.push_back(vertice);
        ++index;
    }
    //save sink vertice in vertices vector
    sinkVertice.node_id = SINK_ID;
    sinkVertice.sinkPathLength = 0;
    vertices.insert(vertices.begin(), sinkVertice);
    vertice.isDirectVertex = false;
    for(auto && mappedNode : m_clusterPathMap)
    {
        extractMappedNodeData(mappedNode, vertice);
        vertices.push_back(vertice);
    }
    //connect vertices
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
    //initialized vertices now calculate paths using dijkstra algorithm
    QVector<quint16> unvisitedVertices(vertices.size());
    std::iota(unvisitedVertices.begin(), unvisitedVertices.end(), 0);
    while(!unvisitedVertices.empty())
    {
        quint16 closestVertexIndex = 0;
        quint16 unvisitedIndex = 0;
        quint16 minDistance = UINT16_MAX;
        for(quint16 i = 0; i < unvisitedVertices.size(); i++)
        {
            if(vertices[unvisitedVertices[i]].sinkPathLength < minDistance)
            {
                closestVertexIndex = unvisitedVertices[i];
                unvisitedIndex = i;
                minDistance = vertices[unvisitedVertices[i]].sinkPathLength;
            }
        }
        quint16 pathLength = UINT16_MAX;
        for(quint16 i = 0; i < vertices[closestVertexIndex].neighbourVerticesIndexes.size(); i++)
        {
            auto currentIndex = vertices[closestVertexIndex].neighbourVerticesIndexes[i];
            if(unvisitedVertices.indexOf(currentIndex) >= 0)
            {
                quint16 distance = minDistance + vertices[closestVertexIndex].neighbourVerticesDistances[i];
                if(vertices[currentIndex].sinkPathLength > distance)
                {
                    vertices[currentIndex].sinkPathLength = distance;
                    vertices[currentIndex].sinkPath = {vertices[closestVertexIndex].sinkPath};
                    vertices[currentIndex].sinkPath.insert(vertices[currentIndex].sinkPath.begin(), vertices[closestVertexIndex].node_id);
                    if(distance < pathLength)
                    {
                        pathLength = distance;
                    }
                }
            }
        }
        unvisitedVertices.erase(unvisitedVertices.begin() + unvisitedIndex);
    }
    return vertices;
}

bool SinkNode::updatePathsAndCreateSyncMsg(const QVector<SinkNode::Vertice> &vertices, QByteArray &updateMsg)
{
    bool updatedPaths = false;
    if(!vertices.isEmpty())
    {
        QJsonArray jsonPaths;
        for(Vertice && vertex : vertices)
        {
            if(!vertex.isDirectVertex)
            {
                //update only the mapped clusters
                MappedClusterNode node;
                node.node_id = vertex.node_id;
                node.layer_id = vertex.layer_id;
                int idx = m_clusterPathMap.indexOf(node);
                if(idx >= 0)
                {
                    //update mapped clusters
                    m_clusterPathMap[idx].sinkPath = vertex.sinkPath;
                    m_clusterPathMap[idx].pathLength = vertex.sinkPathLength;
                    //create json mesg object
                    QJsonArray jsonPath;
                    for(auto && node_id : vertex.sinkPath)
                    {
                        jsonPath.append(QJsonValue(node_id));
                    }
                    QJsonObject jsonPathObject =
                    {
                        {NODE_ID, vertex.node_id},
                        {LAYER_ID, vertex.layer_id},
                        {PATH, jsonPath},
                        {PATH_LENGTH, vertex.sinkPathLength}
                    };
                    jsonPaths.append(QJsonValue(jsonPathObject));
                }
            }
        }
        QJsonObject jsonMsgObject =
        {
           {PATHS, jsonPaths}
        };
        QJsonDocument jsonMsg(jsonMsgObject);
        updateMsg = jsonMsg.toBinaryData();
        updatedPaths = (!updateMsg.isEmpty());
    }
    return updatedPaths;
}


