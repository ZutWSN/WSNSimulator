#include "sinknode.h"
#include "ClusterNode.h"
#include "DragWidget.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>

const quint16 SINK_ID = UINT16_MAX;

SinkNode::SinkNode()
{

}

SinkNode::SinkNode(const QPoint &position, double range):
    m_position(position),
    m_range(range)
{

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
            addedCluster &= static_cast<bool>(connect(clusterNode, SIGNAL(sendDataToSink(DataFrame)), this, SLOT(onReceivedDataFromCluster(DataFrame))));
            if(addedCluster)
            {
                m_inRangeClusters.push_back(cluster);
                clusterNode->setConnectedToSink(this);
                clusterNode->setPathLength(clusterNode->getDistanceFromNode(m_position));
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
                if(cluster->getNumOfConnectedNodes() == 0)
                {
                    static_cast<ClusterNode*>(cluster)->removeFromSyncPath();
                }
            }
        }
    }
    return removed;
}

bool SinkNode::connectToNodeWidget(QWidget *widget)
{
    bool success = false;
    DragWidget *dragWidget = static_cast<DragWidget*>(widget);
    if(dragWidget->getWidgetType() == DragWidget::DragWidgetType::Sink)
    {
        if(dragWidget && !dragWidget->isConnectedToNode())
        {
            if(!m_connectedToWidget)
            {
                success = static_cast<bool>(connect(this, SIGNAL(receivedData(DataFrame)), dragWidget, SLOT(onNodeReceivedData(DataFrame))));
                success &= static_cast<bool>(connect(this, SIGNAL(sendData(DataFrame)), dragWidget, SLOT(onNodeSendData(DataFrame))));
                m_connectedToWidget = success;
                if(m_connectedToWidget)
                {
                    dragWidget->setConnectedToNode(true);
                    m_Widget = widget;
                }
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

void SinkNode::sendNewPaths(quint16 senderLayer)
{
    QByteArray msg;
    if(calculateNetworkPaths(msg))
    {
        DataFrame pathUpdate(msg, DataFrame::RxData::PATH_SYNC, 0, 0, 0);
        ClusterNode::resetBroadCastSyncVector(senderLayer);
        emit broadCastDataToClusters(pathUpdate);
        emit sendData(pathUpdate);
    }
}

void SinkNode::sendSinkRemovedBroadcast()
{
    DataFrame noSinkConnection(QByteArray(), DataFrame::RxData::NO_SINK_CONNECTION, 0, 0, 0);
    ClusterNode::resetBroadCastAllLayers();
    emit broadCastDataToClusters(noSinkConnection);
}

void SinkNode::setPosition(const QPoint &pos)
{
    m_position = pos;
}

void SinkNode::setRange(quint16 range)
{
    m_range = range;
}

quint16 SinkNode::getNumOfInRangeCusters() const
{
    return m_inRangeClusters.size();
}

quint16 SinkNode::getNumOfMappedClusters() const
{
    return m_clusterPathMap.size();
}

quint16 SinkNode::getNumOfConnectedLayers() const
{
    return m_layers.size();
}

DataFrame SinkNode::getLastMsg() const
{
    return m_lastMsg;
}

QPoint SinkNode::getSinkPosition() const
{
    return m_position;
}

double SinkNode::getSinkRange() const
{
    return m_range;
}

bool SinkNode::checkIfHasDirectCluster(NetworkNode *cluster) const
{
    return (m_inRangeClusters.indexOf(cluster) >= 0);
}

bool SinkNode::checkIfHasDirectCluster(quint16 node_id, quint16 layer_id) const
{
    bool hasCluster = false;
    for(auto && directCluster : m_inRangeClusters)
    {
        if(node_id == directCluster->getNodeID())
        {
            if(layer_id == directCluster->getNodeLayer())
            {
                hasCluster = true;
                break;
            }
        }
    }
    return hasCluster;
}

bool SinkNode::checkIfHasMappedCluster(quint16 node_id, quint16 layer_id) const
{
    MappedClusterNode node;
    node.node_id = node_id;
    node.layer_id = layer_id;
    return (m_clusterPathMap.indexOf(node) >= 0);
}

void SinkNode::onReceivedDataFromCluster(const DataFrame &data)
{
    m_lastMsg = data;
    switch(data.getMsgType())
    {
        case DataFrame::RxData::CLUSTER_PATH:
            updateClusterPath(data);
            break;
        case DataFrame::RxData::MOVED_NODE:
        case DataFrame::RxData::REMOVED_NODE:
            removeNode(data.getMsg());
            if(!m_inRangeClusters.isEmpty())
            {
                sendNewPaths(data.getSender().second);
            }
            else//node removed was last in range cluster
            {
                //reset mapped network sinkpaths
                resetMappedSinkPaths();
                //send message to network about last direct cluster disconnection before it gets removed
                DataFrame noSinkConnection(QByteArray(), DataFrame::RxData::NO_SINK_CONNECTION, 0, 0, 0);
                ClusterNode::resetBroadCastSyncVector(data.getSender().second);
                emit broadCastDataToClusters(noSinkConnection);
                emit sendData(noSinkConnection);
            }
            break;
        default:
            break;
    }
    emit receivedData(data);
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
        double pathLength = 0;
        QPoint position;
        QVector<quint16> nodePath;
        QVector<quint16> neighbourIDs;
        QVector<double> neighbourDistances;
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
                        neighbourDistances.push_back(id.toDouble());
                    }
                }
                if(!neighbourDistances.isEmpty())
                {
                    ++parameters;
                }
            }
            else if(key == PATH_LENGTH)
            {
                pathLength = jsonObj[key].toDouble();
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
                int index = checkIfHasMappedCluster(node);
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
                //update mapped neighbours paths
                for(quint16 i = 0; i < neighbourIDs.size(); i++)
                {
                    MappedClusterNode neighbour{neighbourIDs[i], layer_id};
                    int neighbourIndex = m_clusterPathMap.indexOf(neighbour);
                    if(neighbourIndex >= 0)
                    {
                        m_clusterPathMap[neighbourIndex].neighbourIDs.push_back(node_id);
                        m_clusterPathMap[neighbourIndex].neighbourDistances.push_back(neighbourDistances[i]);
                    }
                    else
                    {
                        //dont count the direct cluster connections, they are mapped in separate container
                        if(!checkIfHasDirectCluster(neighbour.node_id, layer_id))
                        {
                            //neighbour not mapped add it
                            neighbour.neighbourIDs.push_back(node_id);
                            neighbour.neighbourDistances.push_back(neighbourDistances[i]);
                            neighbour.sinkPath = nodePath;
                            neighbour.sinkPath.insert(neighbour.sinkPath.begin(), node_id);
                            neighbour.pathLength = pathLength + neighbourDistances[i];
                            m_clusterPathMap.push_back(neighbour);
                        }
                    }
                }
                pathUpdated = true;
            }
        }
    }
    return pathUpdated;
}

int SinkNode::checkIfHasMappedCluster(const MappedClusterNode &node) const
{
    return m_clusterPathMap.indexOf(node);
}

void SinkNode::removeNode(const QByteArray &msg)
{
    //extract from message which node was removed
    //and remove it either from direct clusters or from
    //mapped nodes
    QJsonDocument jsonData = QJsonDocument::fromBinaryData(msg, QJsonDocument::Validate);
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
                for(NetworkNode *directCluster : m_inRangeClusters)
                {
                    ClusterNode *cluster = static_cast<ClusterNode*>(directCluster);
                    if(*cluster == clusterToRemove)
                    {
                        //remove this cluster from in range cluster array
                        //and all mapped nodes neighbours.
                        for(auto && neighbourNode : cluster->getNeighbours())
                        {
                            quint16 i = 0;
                            while(i < m_clusterPathMap.size())
                            {
                                bool nextIndex = true;
                                if(m_clusterPathMap[i].layer_id == cluster->getNodeLayer())
                                {
                                    if(neighbourNode.first == m_clusterPathMap[i].node_id)
                                    {
                                        int idx = m_clusterPathMap[i].neighbourIDs.indexOf(cluster->getNodeID());
                                        if(idx >= 0)
                                        {
                                            m_clusterPathMap[i].neighbourIDs.remove(idx);
                                            m_clusterPathMap[i].neighbourDistances.remove(idx);
                                            if(m_clusterPathMap[i].neighbourIDs.isEmpty())
                                            {
                                                m_clusterPathMap.remove(i);
                                                nextIndex = false;
                                            }
                                        }
                                    }
                                }
                                if(nextIndex)
                                {
                                    ++i;
                                }
                            }
                        }
                        m_inRangeClusters.remove(m_inRangeClusters.indexOf(directCluster));
                        break;
                    }
                }
            }
            else
            {
                MappedClusterNode nodeToRemove;
                nodeToRemove.node_id = node_id;
                nodeToRemove.layer_id = layer_id;
                int idxNodeToRemove = m_clusterPathMap.indexOf(nodeToRemove);
                if(idxNodeToRemove >= 0)
                {
                    nodeToRemove = m_clusterPathMap[idxNodeToRemove];
                    for(auto && neighbourID : nodeToRemove.neighbourIDs)
                    {
                        MappedClusterNode currentNode;
                        currentNode.node_id = neighbourID;
                        currentNode.layer_id = layer_id;
                        int currentIndex = m_clusterPathMap.indexOf(currentNode);
                        if(currentIndex >= 0)
                        {
                            int index = m_clusterPathMap[currentIndex].neighbourIDs.indexOf(nodeToRemove.node_id);
                            if(index >= 0)
                            {
                                m_clusterPathMap[currentIndex].neighbourIDs.remove(index);
                                m_clusterPathMap[currentIndex].neighbourDistances.remove(index);
                                if(m_clusterPathMap[currentIndex].neighbourIDs.isEmpty())
                                {
                                    m_clusterPathMap.remove(currentIndex);
                                }
                            }
                        }
                    }
                    idxNodeToRemove = m_clusterPathMap.indexOf(nodeToRemove);
                    m_clusterPathMap.remove(idxNodeToRemove);
                }
            }
        }
    }
}

void SinkNode::extractNodeData(NetworkNode *node, SinkNode::Vertice &vertice) const
{
    vertice.node_id = node->getNodeID();
    vertice.layer_id = node->getNodeID();
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
    sinkVertice.node_id = SINK_ID;
    sinkVertice.sinkPathLength = 0;
    sinkVertice.neighbourVerticesIndexes = QVector<quint16>(m_inRangeClusters.size());
    //create sink start vertice and put its neighbours in vertices vector
    for(NetworkNode* directNode : m_inRangeClusters)
    {
        //the sink will be inserted at the beginning of vertices vector
        //so all of its neighbour vertices will be increased by one
        sinkVertice.neighbourVerticesIndexes[index] = index + 1;
        sinkVertice.neighbourVerticesDistances.push_back(directNode->getDistanceFromNode(m_position));
        //add sink vertice to direct cluster vertices
        vertice.neighbourVerticesIndexes.push_back(sinkIndex);
        vertice.neighbourVerticesDistances.push_back(directNode->getDistanceFromNode(m_position));
        extractNodeData(directNode, vertice);

        vertices.push_back(vertice);
        vertice.neighbourVerticesIndexes.clear();
        vertice.neighbourVerticesDistances.clear();
        vertice.neighbours.clear();
        ++index;
    }
    //save sink vertice in vertices vector
    vertices.insert(vertices.begin(), sinkVertice);
    vertice.isDirectVertex = false;
    for(auto && mappedNode : m_clusterPathMap)
    {
        //neighbours extraction error
        extractMappedNodeData(mappedNode, vertice);
        vertices.push_back(vertice);
        vertice.neighbourVerticesIndexes.clear();
        vertice.neighbourVerticesDistances.clear();
        vertice.neighbours.clear();
    }
    //connect vertices
    for(auto && v : vertices)
    {
        for(auto && idAndDist : v.neighbours)
        {
            vertice.node_id = idAndDist.first;
            int idx = vertices.indexOf(vertice);
            v.neighbourVerticesIndexes.push_back(idx);
            v.neighbourVerticesDistances.push_back(idAndDist.second);
        }
    }
    //initialized vertices now calculate paths using dijkstra algorithm
    QVector<quint16> unvisitedVertices(vertices.size());
    std::iota(unvisitedVertices.begin(), unvisitedVertices.end(), 0);
    while(!unvisitedVertices.empty())
    {
        quint16 closestVertexIndex = 0;
        quint16 unvisitedIndex = 0;
        double minDistance = UINT64_MAX;
        for(quint16 i = 0; i < unvisitedVertices.size(); i++)
        {
            if(vertices[unvisitedVertices[i]].sinkPathLength < minDistance)
            {
                closestVertexIndex = unvisitedVertices[i];
                unvisitedIndex = i;
                minDistance = vertices[unvisitedVertices[i]].sinkPathLength;
            }
        }
        double pathLength = UINT64_MAX;
        for(quint16 i = 0; i < vertices[closestVertexIndex].neighbourVerticesIndexes.size(); i++)
        {
            auto currentIndex = vertices[closestVertexIndex].neighbourVerticesIndexes[i];
            if(unvisitedVertices.indexOf(currentIndex) >= 0)
            {
                double distance = minDistance + vertices[closestVertexIndex].neighbourVerticesDistances[i];
                if(vertices[currentIndex].sinkPathLength > distance)
                {
                    vertices[currentIndex].sinkPathLength = distance;
                    vertices[currentIndex].sinkPath = {vertices[closestVertexIndex].sinkPath};
                    if(vertices[closestVertexIndex].node_id != SINK_ID) // don't add sink id to message paths
                    {
                       vertices[currentIndex].sinkPath.insert(vertices[currentIndex].sinkPath.begin(), vertices[closestVertexIndex].node_id);
                    }
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
        for(auto && vertex : vertices)
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

void SinkNode::resetMappedSinkPaths()
{
    for(MappedClusterNode &mappedNode : m_clusterPathMap)
    {
        mappedNode.sinkPath.clear();
        mappedNode.pathLength = UINT64_MAX;
    }
}


