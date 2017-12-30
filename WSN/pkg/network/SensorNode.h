#ifndef SENSORNODE_H
#define SENSORNODE_H
#include "NetworkNode.h"

class SensorNode : public NetworkNode
{
    Q_OBJECT
public:
    SensorNode(quint16 node_id);
    SensorNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position);
    //add for this and cluster node assignment operators and copy constructor definitions, using base class ones inside
    ~SensorNode();

    bool connectToNode(NetworkNode *node);
    bool disconnectFromNode(NetworkNode *node);
    bool sendDataToCluster(const QByteArray &data);

    NetworkNode::NodeType getNodeType() const;
    quint16 getClusterID() const;
    QPoint getClusterPosition() const;
    bool isConnectedToCluster() const;
public slots:
    void onReceivedDataFromCluster(const DataFrame &rxData);
signals:
    void clusterDataSend(const QByteArray &data);
private:
    void processNewData(const DataFrame &rxData);
private:
    quint16 m_cluster_id;
    QPoint m_clusterPosition;
    bool m_connectedToCluster;
};

#endif // SENSORNODE_H
