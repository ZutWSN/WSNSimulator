#include "ClusterNode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include "SensorNode.h"

QVector<ClusterNode::SyncLayer> ClusterNode::m_networkClusterSyncNodes;

ClusterNode::ClusterNode(quint16 node_id) :
    NetworkNode(node_id),
    m_sensorDataCounter(0),
    m_neighbourPathsCounter(0),
    m_pathLength(UINT64_MAX),
    m_state(ClusterStates::CREATED)
{

}

ClusterNode::ClusterNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position):
    NetworkNode(node_id, range, layer_id, node_position),
    m_sensorDataCounter(0),
    m_neighbourPathsCounter(0),
    m_pathLength(0),
    m_state(ClusterStates::CREATED)
{

}

ClusterNode::~ClusterNode()
{

}

bool ClusterNode::connectToNode(NetworkNode *node)
{
    bool connected = false;
    if(node)
    {
        if(node->getNodeType() == NetworkNode::NodeType::Cluster)
        {
            connected = NetworkNode::connectToNode(node);
            addToSyncNodePath(this);
            addToSyncNodePath(node);
            if(static_cast<ClusterNode*>(node)->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK)
            {
                addDirectClusterConnection(node->getNodeID());
            }
        }
        else if(node->getNodeType() == NetworkNode::NodeType::Sensor)
        {
            connected = node->connectToNode(this);
        }
    }
    return connected;
}

bool ClusterNode::sendSinkPathReq()
{
    bool seekingPath = false;
    if(m_state != ClusterStates::CONNECTED_TO_SINK)
    {
        DataFrame frame(QByteArray(), DataFrame::RxData::NEIGHBOUR_PATH_REQ, 0, m_layer_id, m_node_id);
        m_state = ClusterStates::PATH_SEEKING;
        sendData(frame);
    }
    return seekingPath;
}

NetworkNode::NodeType ClusterNode::getNodeType() const
{
    return NodeType::Cluster;
}

bool ClusterNode::removeFromSyncPath()
{
    bool removed = false;
    int layerIndex = m_networkClusterSyncNodes.indexOf(SyncLayer(m_layer_id));
    int nodeIndex = m_networkClusterSyncNodes[layerIndex].syncNodes.indexOf(SyncNode(this));
    if(nodeIndex >= 0)
    {
        m_networkClusterSyncNodes[layerIndex].syncNodes.remove(nodeIndex);
        removed = true;
    }
    return removed;
}

void ClusterNode::resetBroadCastSyncVector(quint16 layer_id)
{
    int layer_idx = m_networkClusterSyncNodes.indexOf(SyncLayer(layer_id));
    for(auto && syncNode : m_networkClusterSyncNodes[layer_idx].syncNodes)
    {
        syncNode.visited = false;
    }
}

void ClusterNode::onReceivedDataFromSensor(const QByteArray &data)
{
    m_mesgData += data;
    ++m_sensorDataCounter;
    if(m_sensorDataCounter >= m_sensors.size())
    {
        if(m_state == ClusterStates::CONNECTED_TO_SINK)
        {
            DataFrame frame(m_mesgData, DataFrame::RxData::NEW_DATA, 0, 0, 0);
            m_sensorDataCounter = 0;
            emit sendDataToSink(frame);
        }
        else if(!m_sinkPath.isEmpty() && m_pathLength > 0)
        {
            DataFrame frame(m_mesgData, DataFrame::RxData::NEW_DATA, m_sinkPath[0], m_layer_id, m_node_id);
            frame.setPath(m_sinkPath);
            m_sensorDataCounter = 0;
            sendData(frame);
        }
    }
}

bool ClusterNode::disconnectFromNode(NetworkNode *node)
{
    bool disconnected = false;
    if(node)
    {
        if(node->getNodeType() == NetworkNode::NodeType::Sensor)
        {
            if(checkIfConnectedToSensor(node))
            {
                if(node->disconnectFromNode(this))
                {
                    disconnected = true;
                    m_sensors.remove(m_sensors.indexOf(node));
                }
            }
        }
        else if(node->getNodeType() == NetworkNode::NodeType::Cluster)
        {
            disconnected = NetworkNode::disconnectFromNode(node);
            if(getNumOfConnectedNodes() == 0 && m_state != ClusterStates::CONNECTED_TO_SINK)
            {
                removeFromSyncPath();
            }
        }
    }
    return disconnected;
}

bool ClusterNode::disconnectFromNetwork()
{
    bool disconnected = NetworkNode::disconnectFromNetwork();
    for(NetworkNode *sensor : m_sensors)
    {
        disconnected &= sensor->disconnectFromNode(this);
        disconnected &= removeSensor(sensor);
    }
    int layerIndex = m_networkClusterSyncNodes.indexOf(SyncLayer(m_layer_id));
    int nodeIndex = m_networkClusterSyncNodes[layerIndex].syncNodes.indexOf(SyncNode(this));
    if(nodeIndex >= 0)
    {
        m_networkClusterSyncNodes[layerIndex].syncNodes.remove(nodeIndex);
    }
    return disconnected;
}

bool ClusterNode::addNode(NetworkNode *node)
{
    bool addedNode = false;
    if(node)
    {
        if(node->getNodeType() == NetworkNode::NodeType::Cluster)
        {
            addedNode = NetworkNode::addNode(node);
        }
        else if(node->getNodeType() == NetworkNode::NodeType::Sensor)
        {
            if(node->getNodeLayer() == m_layer_id)
            {
                if(!checkIfConnectedToSensor(node))
                {
                    m_sensors.push_back(node);
                    addedNode = true;
                }
            }
        }
    }
    return addedNode;
}

bool ClusterNode::removeSensor(NetworkNode *sensor)
{
    bool removed = false;
    int idx = m_sensors.indexOf(sensor);
    if(idx >= 0)
    {
        if(!static_cast<SensorNode*>(sensor)->isConnectedToCluster())
        {
            m_sensors.remove(idx);
            removed = true;
        }
    }
    return removed;
}

bool ClusterNode::setSinkPath(const QVector<quint16> &path)
{
    bool success = false;
    if(!path.isEmpty())
    {
        m_sinkPath = path;
        success = true;
    }
    return success;
}

void ClusterNode::setPathLength(double length)
{
    m_pathLength = length;
}

bool ClusterNode::setConnectedToSink(SinkNode *sink)
{
    bool changedState = false;
    if(sink)
    {
        if(sink->checkIfHasDirectCluster(this))
        {
            m_state = ClusterStates::CONNECTED_TO_SINK;
            addToSyncNodePath(this);
            changedState = true;
        }
    }
    return changedState;
}

quint16 ClusterNode::getNumOfSensors() const
{
    return m_sensors.size();
}

quint16 ClusterNode::getNumOfSensorPendingMsgs() const
{
    return m_sensorDataCounter;
}

ClusterNode::ClusterStates ClusterNode::getCurrentState() const
{
    return m_state;
}

QVector<quint16> ClusterNode::getSinkPath() const
{
    return m_sinkPath;
}

bool ClusterNode::checkIfConnectedToSensor(NetworkNode *sensor) const
{
    bool connected = false;
    for(NetworkNode* connected_sensor : m_sensors)
    {
        if(sensor == connected_sensor)
        {
            connected = true;
            break;
        }
    }
    return connected;
}

void ClusterNode::processNewData(const DataFrame &rxData)
{
    bool sendToSink = false;
    bool sendTxData = false;
    bool sendDataToSensors = false;
    NetworkNode::processNewData(rxData);
    DataFrame txData(rxData);
    txData.setSender(qMakePair(m_node_id, m_layer_id));
    if(rxData.getMsgType() == DataFrame::RxData::NEIGHBOUR_PATH_REQ)
    {
        txData.setMsgType(DataFrame::RxData::NEIGHBOUR_PATH);
        txData.setDestination(rxData.getSender());
        txData.setPath({rxData.getSender().first});
        QVector<quint16> path;
        if(!m_sinkPath.isEmpty() || (m_state == ClusterStates::CONNECTED_TO_SINK))
        {
            path = m_sinkPath;
            path.insert(path.begin(), m_node_id);
        }
        QByteArray pathMsg;
        if(createClusterPathMsg(path, pathMsg))
        {
            txData.setMsg(pathMsg);
            sendTxData = true;
        }
    }
    else if(rxData.getMsgType() == DataFrame::RxData::PATH_SYNC)
    {
        if(!checkIfVisitedThisNode())
        {           
            extractPathFromMsg(rxData.getMsg());
            txData.addVisitedNode(qMakePair(m_node_id, m_layer_id));
            setThisNodeVisited();
            if(!checkIfAllSyncNodesVisited())
            {
                sendTxData = true;
            }

        }
    }
    else if(rxData.getMsgType() == DataFrame::RxData::NO_SINK_CONNECTION)
    {
        //network broadcast
        if(!checkIfVisitedThisNode())
        {
            txData.addVisitedNode(qMakePair(m_node_id, m_layer_id));
            setThisNodeVisited();
            if(m_state != ClusterStates::CONNECTED_TO_SINK)
            {
                //clear sink path and its length, change state to disconnected
                if(m_state != ClusterStates::DISCONNECTED)
                {
                    m_sinkPath.clear();
                    m_pathLength = UINT64_MAX;
                    m_state = ClusterStates::DISCONNECTED;
                    m_directNodesConnections.clear();
                }
            }
            if(!checkIfAllSyncNodesVisited())
            {
                sendTxData = true;
            }
        }
    }
    else if(rxData.getMsgType() == DataFrame::RxData::DIRECT_CLUSTER_REMOVED)
    {
        if(!checkIfVisitedThisNode())
        {
            if(m_state != ClusterStates::CONNECTED_TO_SINK)
            {
                if(removeDirectClusterConnection(rxData.getSender().first))
                {
                    //connection has been removed inform other nodes
                    txData.setSender(rxData.getSender());
                    //update node state and reset sinkPath
                    if(m_directNodesConnections.isEmpty())
                    {
                        m_state = ClusterStates::DISCONNECTED;
                        m_sinkPath.clear();
                        m_pathLength = UINT64_MAX;
                    }
                    sendTxData = true;
                }
            }
            setThisNodeVisited();
        }
    }
    else
    {
        QPair<quint16, quint16> destination = rxData.getDestination();
        if(destination.second == m_layer_id)
        {
            if(destination.first == m_node_id)
            {
                //now start processing for forwarding or broadcasting
                if(rxData.isFinalDestination(txData.getSender()))
                {
                    switch(rxData.getMsgType())
                    {
                        case DataFrame::RxData::NEW_DATA:
                        case DataFrame::RxData::REMOVED_NODE:
                            if(m_state == ClusterStates::CONNECTED_TO_SINK)
                            {
                                sendToSink = true;
                            }
                            else
                            {
                                txData.setMsgType(DataFrame::RxData::SENSOR_BROADCAST);
                                sendDataToSensors = true;
                            }
                            break;
                        case DataFrame::RxData::NEIGHBOUR_PATH:
                            if(m_neighbourPathsCounter < m_connectedNodes.size())
                            {
                                QVector<quint16> path;
                                double pathLength = getPathFromNeighbourMsg(rxData, path);
                                if(pathLength > 0)
                                {
                                    if(pathLength < m_pathLength)
                                    {
                                        m_pathLength = pathLength;
                                        m_sinkPath = path;
                                    }
                                    ++m_neighbourPathsCounter;
                                    if(m_neighbourPathsCounter == m_connectedNodes.size())
                                    {
                                        m_neighbourPathsCounter = 0;
                                        m_state = ClusterStates::CONNECTED;
                                        QByteArray clusterPathMsg;
                                        //send newly found sinkpath to sink for it to recognize that new cluster has been
                                        //added to its cluster network and so that sink can send messages to it
                                        if(createClusterPathMsg(m_sinkPath, clusterPathMsg))
                                        {
                                            DataFrame frame(clusterPathMsg, DataFrame::RxData::CLUSTER_PATH, m_sinkPath[0], m_layer_id, m_node_id);
                                            frame.setPath(m_sinkPath);
                                            txData = frame;
                                            sendTxData = true;
                                        }
                                    }
                                }

                            }
                            break;
                        case DataFrame::RxData::CLUSTER_PATH:
                            if(m_state == ClusterStates::CONNECTED_TO_SINK)
                            {
                                sendToSink = true;
                            }
                        break;
                        default:
                            break;
                    }
                }
                else
                {
                    //forward data
                    QPair<quint16, quint16> nextNode = rxData.getNextChainNode(m_node_id, m_layer_id);
                    if(checkIfConnectedToNode(nextNode))
                    {
                        txData.setDestination(nextNode);
                        sendTxData = true;
                    }
                }
            }
        }
    }
    if(sendToSink)
    {
        emit sendDataToSink(txData);
    }
    else if(sendTxData)
    {
        sendData(txData);
    }
    else if(sendDataToSensors)
    {
        emit broadcastDataToSensors(txData);
    }
}

double ClusterNode::getPathFromNeighbourMsg(const DataFrame &rxData, QVector<quint16> &path)
{
    double pathLength= 0;
    QJsonDocument jsonData = QJsonDocument::fromBinaryData(rxData.getMsg(), QJsonDocument::Validate);
    if(!jsonData.isNull())
    {
        QJsonObject jsonObj = jsonData.object();
        quint16 node_id, layer_id;
        QVector<quint16> nodePath;
        auto keys = jsonObj.keys();
        for(auto && key : keys)
        {
            if(key == NODE_ID)
            {
                node_id = static_cast<quint16>(jsonObj[key].toInt());
            }
            else if(key == LAYER_ID)
            {
                layer_id = static_cast<quint16>(jsonObj[key].toInt());
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
            }
            else if(key == PATH_LENGTH)
            {
                pathLength = jsonObj[key].toDouble();
            }
            else if(key == DIRECT_CLUSTERS)
            {
                for(auto && id: jsonObj[key].toArray())
                {
                    if(id.isDouble())
                    {
                        addDirectClusterConnection(static_cast<quint16>(id.toInt()));
                    }
                }
            }
        }
        if(node_id == rxData.getSender().first && layer_id == rxData.getSender().second)
        {
            double distance = getDistanceFromConnectedNode(node_id);
            if(distance > 0 && pathLength > 0 && pathLength < UINT64_MAX)
            {
                path = std::move(nodePath);
                pathLength += distance;
            }
        }
    }
    return pathLength;
}

//extracts new path for node based on received sink message which modifies already existing path
//happens when new cluster is added to network or if it is removed
bool ClusterNode::extractPathFromMsg(const QByteArray &pathMsg)
{
    bool pathExtracted = false;
    QJsonDocument jsonData = QJsonDocument::fromBinaryData(pathMsg, QJsonDocument::Validate);
    if(!jsonData.isNull())
    {
        QJsonObject jsonObj = jsonData.object();
        quint16 node_id, layer_id;
        QVector<quint16> nodePath;
        double pathLength = 0;
        auto keys = jsonObj.keys();
        for(auto && key : keys)
        {
            if(key == PATHS)
            {
                if(jsonObj[key].isArray())
                {
                    auto listOfPaths = jsonObj[key].toArray();
                    for(auto && path : listOfPaths)
                    {
                        if(path.isObject())
                        {
                            auto pathObj = path.toObject();
                            auto pathKeys = pathObj.keys();
                            for(auto && pKey : pathKeys)
                            {
                                if(pKey == NODE_ID)
                                {
                                    node_id = static_cast<quint16>(pathObj[pKey].toInt());
                                }
                                else if(pKey == LAYER_ID)
                                {
                                    layer_id = static_cast<quint16>(pathObj[pKey].toInt());
                                }
                                else if(pKey == PATH)
                                {
                                    nodePath.clear();
                                    for(auto && id: pathObj[pKey].toArray())
                                    {
                                        if(id.isDouble())
                                        {
                                            nodePath.push_back(static_cast<quint16>(id.toInt()));
                                        }
                                    }
                                }
                                else if(pKey == PATH_LENGTH)
                                {
                                    pathLength = pathObj[pKey].toDouble();
                                }
                            }
                        }
                        if(m_node_id == node_id && m_layer_id == layer_id)
                        {
                            //for now path can be only sent by neighbour but later can be send by sink
                            //so it has to be checked and calculated differently
                            m_sinkPath = std::move(nodePath);
                            m_pathLength = pathLength;
                            pathExtracted = true;
                            break;
                        }
                    }
                }
            }
            if(pathExtracted)
            {
                break;
            }
        }

    }
    return pathExtracted;
}

bool ClusterNode::createClusterPathMsg(const QVector<quint16> &path, QByteArray &msg)
{
    bool created = false;
    QJsonArray jsonPath;
    QJsonArray jsonNeighbourIDs;
    QJsonArray jsonNeighbourDistances;
    QJsonArray jsonDirectClusters;
    for(auto && node : path)
    {
        jsonPath.append(QJsonValue(node));
    }
    for(auto && neighbour : NetworkNode::getNeighbours())
    {
        jsonNeighbourIDs.append(QJsonValue(neighbour.first));
        jsonNeighbourDistances.append(QJsonValue(neighbour.second));
    }

    for(auto && directCluster : m_directNodesConnections)
    {
        jsonDirectClusters.append(QJsonValue(directCluster));
    }

    if(m_state == ClusterStates::CONNECTED_TO_SINK)
    {
       jsonDirectClusters.append(QJsonValue(m_node_id));
    }

    QJsonObject clusterPathObj =
    {
        {NODE_ID, m_node_id},
        {LAYER_ID, m_layer_id},
        {NODE_POSITION_X, m_node_position.x()},
        {NODE_POSITION_Y, m_node_position.y()},
        {PATH, jsonPath},
        {PATH_LENGTH, m_pathLength},
        {NEIGHBOURS_IDS, jsonNeighbourIDs},
        {NEIGHBOURS_DISTANCES, jsonNeighbourDistances},
        {DIRECT_CLUSTERS, jsonDirectClusters}
    };
    QJsonDocument jsonMsg(clusterPathObj);
    msg = jsonMsg.toBinaryData();
    if(!msg.isEmpty() && !msg.isNull())
    {
        created = true;
    }
    return created;
}

bool ClusterNode::checkIfVisitedThisNode() const
{
    bool visited = false;
    int layer_idx = m_networkClusterSyncNodes.indexOf(SyncLayer(m_layer_id));
    int nodeIndex = m_networkClusterSyncNodes[layer_idx].syncNodes.indexOf(SyncNode(this));
    if(nodeIndex >= 0)
    {
        visited = m_networkClusterSyncNodes[layer_idx].syncNodes[nodeIndex].visited;
    }
    return visited;
}

bool ClusterNode::setThisNodeVisited()
{
    bool setVisited = false;
    int layer_idx = m_networkClusterSyncNodes.indexOf(SyncLayer(m_layer_id));
    int nodeIndex = m_networkClusterSyncNodes[layer_idx].syncNodes.indexOf(SyncNode(this));
    if(nodeIndex >= 0)
    {
        m_networkClusterSyncNodes[layer_idx].syncNodes[nodeIndex].visited = true;
        setVisited = true;
    }
    return setVisited;
}

bool ClusterNode::checkIfAllSyncNodesVisited() const
{
    bool allVisited = true;
    //get current layer index
    int layer_idx = m_networkClusterSyncNodes.indexOf(SyncLayer(m_layer_id));
    for(auto && syncNode : m_networkClusterSyncNodes[layer_idx].syncNodes)
    {
        if(!syncNode.visited)
        {
            allVisited = false;
            break;
        }
    }
    return allVisited;
}

bool ClusterNode::addToSyncNodePath(NetworkNode *node)
{
    bool added = false;
    SyncNode syncNode(static_cast<ClusterNode*>(node));
    int layer_idx = m_networkClusterSyncNodes.indexOf(SyncLayer(m_layer_id));
    if(layer_idx < 0)
    {
       m_networkClusterSyncNodes.push_back(SyncLayer(m_layer_id));
       layer_idx = m_networkClusterSyncNodes.size() - 1;
    }
    int nodeIndex = m_networkClusterSyncNodes[layer_idx].syncNodes.indexOf(syncNode);
    if(nodeIndex < 0)
    {
        m_networkClusterSyncNodes[layer_idx].syncNodes.push_back(syncNode);
        added = true;
    }
    return added;
}

void ClusterNode::addDirectClusterConnection(quint16 node_id)
{
    if(m_directNodesConnections.indexOf(node_id) < 0)
    {
        m_directNodesConnections.push_back(node_id);
    }
}

bool ClusterNode::removeDirectClusterConnection(quint16 node_id)
{
    bool removed = false;
    int index = m_directNodesConnections.indexOf(node_id);
    if(index >= 0)
    {
        m_directNodesConnections.remove(index);
        removed = true;
    }
    return removed;
}

