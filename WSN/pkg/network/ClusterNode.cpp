#include "ClusterNode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString LAYER_ID  = "Layer_ID";
const QString NODE_ID   = "Node_ID";
const QString PATH      = "Path";
const QString PATHS     = "Paths";

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
            if(node->connectToNode(this))
            {
                connected = true;
                m_sensors.push_back(node);
            }
        }
    }
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
    if(m_sensorDataCounter == m_sensors.size())
    {
        DataFrame frame(m_mesgData, DataFrame::RxData::NEW_DATA, m_sinkPath[0], m_layer_id, m_node_id);
        frame.setPath(m_sinkPath);
        sendData(frame);
        m_sensorDataCounter = 0;
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

quint16 ClusterNode::getNumOfSensors() const
{
    return m_sensors.size();
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
    if(getMsgType() == DataFrame::RxData::NEIGHBOUR_PATH_REQ)
    {
        txData.setMsgType(DataFrame::RxData::NEIGHBOUR_PATH);
        txData.setDestination(rxData.getSender());
        QVector<quint16> path = {rxData.getSender()};
        txData.setPath(path);
        sendData(txData);
        break;
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
                            m_sinkPath = extractPathFromMsg(rxData.getMsg());
                            break;
                        case DataFrame::RxData::NEW_DATA:
                            txData.setMsgType(DataFrame::RxData::SENSOR_BROADCAST);
                            emit broacastDataToSensors(txData.getMsg());
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
                                        if(createClusterPathMsg(clusterPathMsg))
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
                    QPair<quint16, quint16> nextNode = rxData.getNextChainNode(m_node_id);
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
    if(jsonData)
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
                pathLength = static_cast<quint16>(path[key].toInt());
            }
        }
        if(node_id == rxData.getSender().first && layer_id == rxData.getSender().second)
        {
            quint16 distance = getDistanceFromConnectedNode(node_id);
            if(distance > 0 && pathLength > 0)
            {
                m_sinkPath.clear();
                path = std::move(nodePath);
                path.insert(path.begin(), node_id);
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
    if(jsonData)
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
                            auto pathKeys = path.toObject().keys();
                            for(auto && pKey : pathKeys)
                            {
                                if(pKey == NODE_ID)
                                {
                                    node_id = static_cast<quint16>(path[key].toInt());
                                }
                                else if(pKey == LAYER_ID)
                                {
                                    layer_id = static_cast<quint16>(path[key].toInt());
                                }
                                else if(pKey == PATH)
                                {
                                    nodePath.clear();
                                    for(auto && id: path[key].toArray())
                                    {
                                        if(id.isDouble())
                                        {
                                            nodePath.push_back(static_cast<quint16>(id.toInt()));
                                        }
                                    }
                                }
                                else if(pKey == PATH_LENGTH)
                                {
                                    pathLength = static_cast<quint16>(path[key].toInt());
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

bool ClusterNode::createClusterPathMsg(QByteArray &msg)
{
    bool created = false;
    QJsonArray path = QJsonArray::fromVariantList(m_sinkPath);
    QJsonObject clusterPathObj =
    {
        {NODE_ID, m_node_id},
        {LAYER_ID, m_layer_id},
        {PATH, path},
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

