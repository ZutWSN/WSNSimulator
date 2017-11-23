#include "ClusterNode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString LAYER_ID = "Layer_ID";
const QString NODE_ID = "Node_ID";
const QString PATH = "Path";

ClusterNode::ClusterNode(quint16 node_id) :
    NetworkNode(node_id)
{

}

ClusterNode::ClusterNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position):
    NetworkNode(node_id, range, layer_id, node_position),
    m_sensorDataCounter(0)
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
    QPair<quint16, quint16> destination = rxData.getDestination();
    if(destination.second == m_layer_id)
    {
        if(destination.first == m_node_id)
        {
            //send back that message received
            DataFrame txData(rxData);
            //now start processing for forwarding or broadcasting
            txData.setSender(qMakePair(m_node_id, m_layer_id));
            if(rxData.isFinalDestination(txData.getSender()))
            {
                if(rxData.getMsgType() == DataFrame::RxData::PATH_SYNC)
                {
                    bool extractNewPathFromMsg(rxData.getMsg());
                }
                else if(rxData.getMsgType() == DataFrame::RxData::NEW_DATA)
                {
                    txData.setMsgType(DataFrame::RxData::SENSOR_BROADCAST);
                    emit broacastDataToSensors(rxData);
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

bool ClusterNode::extractNewPathFromMsg(const QByteArray &pathMsg)
{
    bool msgExtracted = false;
    QJsonDocument jsonData = QJsonDocument::fromBinaryData(pathMsg, QJsonDocument::Validate);
    if(jsonData)
    {
        QJsonObject jsonObj = jsonData.object();
        quint16 node_id, layer_id;
        QVector<quint16> path;
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
                for(auto && array_elem : jsonObj[key].toArray())
                {
                    if(array_elem.isDouble())
                    {
                        qDebug() << "Json double : " << array_elem.toInt() << "\n";
                    }
                    else if(array_elem.isObject())
                    {
                        readAndPrintJsonObject(array_elem.toObject());
                    }
                }
            }
        }


    }
    /* extract new path from QByteArray message, define message text format
     * np. Parse this Json, it has all data regarding netwok paths
     * JSon:
     * {
        "Sink_ID": "0",
        "Paths": [
        { "Node_ID":"0", "Layer_ID":"2", "Path":[ "0", "1", "2" ] },
        { "Node_ID":"1", "Layer_ID":"2", "Path":[ "1", "2", "0" ] },
        { "Node_ID":"2", "Layer_ID":"2", "Path":[ "2", "1", "0" ] }
    */
    return msgExtracted;
}

