#include "ClusterNode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include "SensorNode.h"

ClusterNode::ClusterNode(quint16 node_id) :
    NetworkNode(node_id),
    m_sensorDataCounter(0),
    m_neighbourPathsCounter(0),
    m_pathLength(0),
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
    if(m_state == ClusterStates::CREATED)
    {
        DataFrame frame(QByteArray(), DataFrame::RxData::NEIGHBOUR_PATH_REQ, 0, m_layer_id, m_node_id);
        if(sendData(frame))
        {
            seekingPath = true;
            m_state = ClusterStates::PATH_SEEKING;
        }
    }
    return seekingPath;
}

NetworkNode::NodeType ClusterNode::getNodeType() const
{
    return NodeType::Cluster;
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
                if(checkIfConnectedToSensor(node))
                {
                    m_sensors.push_back(node);
                    addedNode = true;
                }
            }
        }
    }
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

void ClusterNode::setPathLength(quint16 length)
{
    m_pathLength = length;
}

bool ClusterNode::setConnectedToSink(SinkNode *sink)
{
    bool changedState = false;
    if(sink)
    {
        if(sink->checkIfHasCluster(this))
        {
            m_state = ClusterStates::CONNECTED_TO_SINK;
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
    NetworkNode::processNewData(rxData);
    DataFrame txData(rxData);
    txData.setSender(qMakePair(m_node_id, m_layer_id));
    if(txData.getMsgType() == DataFrame::RxData::NEIGHBOUR_PATH_REQ)
    {
        txData.setMsgType(DataFrame::RxData::NEIGHBOUR_PATH);
        txData.setDestination(rxData.getSender());
        txData.setPath({rxData.getSender().first});
        QVector<quint16> path = m_sinkPath;
        path.insert(path.begin(), m_node_id);
        QByteArray pathMsg;
        if(createClusterPathMsg(path, pathMsg))
        {
            txData.setMsg(pathMsg);
            sendData(txData);
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
                        case DataFrame::RxData::PATH_SYNC:
                            extractPathFromMsg(rxData.getMsg());
                            break;
                        case DataFrame::RxData::NEW_DATA:
                        case DataFrame::RxData::REMOVED_NODE:
                            if(m_state == ClusterStates::CONNECTED_TO_SINK)
                            {
                                emit sendDataToSink(txData);
                            }
                            else
                            {
                                txData.setMsgType(DataFrame::RxData::SENSOR_BROADCAST);
                                emit broadcastDataToSensors(txData);
                            }
                            break;
                        case DataFrame::RxData::NEIGHBOUR_PATH:
                            if(m_neighbourPathsCounter < m_connectedNodes.size())
                            {
                                QVector<quint16> path;
                                quint16 pathLength = getPathFromNeighbourMsg(rxData, path);
                                if(pathLength > 0)
                                {
                                    if(pathLength < m_pathLength || m_pathLength == 0)
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
                                            sendData(frame);
                                        }
                                    }
                                }

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
                        sendData(txData);
                    }
                }
            }
        }
    }
}

quint16 ClusterNode::getPathFromNeighbourMsg(const DataFrame &rxData, QVector<quint16> &path)
{
    quint16 pathLength= 0;
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
                pathLength = static_cast<quint16>(jsonObj[key].toInt());
            }
        }
        if(node_id == rxData.getSender().first && layer_id == rxData.getSender().second)
        {
            quint16 distance = getDistanceFromConnectedNode(node_id);
            if(distance > 0 && pathLength > 0)
            {
                m_sinkPath.clear();
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
        quint16 pathLength = 0;
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
                                    for(auto && id: pathObj[key].toArray())
                                    {
                                        if(id.isDouble())
                                        {
                                            nodePath.push_back(static_cast<quint16>(id.toInt()));
                                        }
                                    }
                                }
                                else if(pKey == PATH_LENGTH)
                                {
                                    pathLength = static_cast<quint16>(pathObj[pKey].toInt());
                                }
                            }
                        }
                        if(m_node_id == node_id && m_layer_id == layer_id)
                        {
                            //for now path can be only sent by neighbour but later can be send by sink
                            //so it has to be checked and calculeted differently
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
    for(auto && node : path)
    {
        jsonPath.append(QJsonValue(node));
    }
    QJsonObject clusterPathObj =
    {
        {NODE_ID, m_node_id},
        {LAYER_ID, m_layer_id},
        {NODE_POSITION_X, m_node_position.x()},
        {NODE_POSITION_Y, m_node_position.y()},
        {PATH, jsonPath},
        {PATH_LENGTH, m_pathLength}
    };
    QJsonDocument jsonMsg(clusterPathObj);
    msg = jsonMsg.toBinaryData();
    if(!msg.isEmpty() && !msg.isNull())
    {
        created = true;
    }
    return created;
}

