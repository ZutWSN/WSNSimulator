#ifndef NETWORKNODE_H
#define NETWORKNODE_H
#include "DataFrame.h"
#include <QObject>
#include <QPoint>
#include <QVector>

class NetworkNode : public QObject
{
    Q_OBJECT
public:
    enum NodeType
    {
        NoType  =   0,
        Sensor =    1,
        Cluster =   2
    };

    NetworkNode(quint16 node_id = 0);
    NetworkNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position);
    NetworkNode(const NetworkNode &other);
    NetworkNode& operator=(const NetworkNode &rhs);
    //maybe add move constructor and move assignment operator
    virtual ~NetworkNode();

    bool sendData(const DataFrame &txData);
    bool addNode(NetworkNode *node);
    virtual bool connectToNode(NetworkNode *node);
    virtual bool disconnectFromNode(NetworkNode *node);
    bool connectToNodeWidget(QWidget *widget);
    bool disconnectFromWidget();

    void setNodeID(quint16 node_id);
    void setLayer(qint16 layer_id);
    void setNodeRange(quint16 range);

    void setNodePosition(const QPoint &position);

    quint16 getNodeID() const;
    qint16 getNodeLayer() const;
    QPoint getNodePostion() const;
    quint16 getNodeRange() const;
    bool getSendDataReceived() const;
    virtual NetworkNode::NodeType getNodeType() const;
    bool checkIfInRange(const QPoint &position) const;
    quint16 getDistanceFromNode(const QPoint &position) const;
    quint16 getDistanceFromConnectedNode(quint16 node_id) const;
    bool checkIfConnectedToNode(NetworkNode *node) const;
    bool checkIfConnectedToNode(const QPair<quint16, quint16> &node) const;
    quint16 getNumOfPendingDataFrames() const;
public slots:
    void onReceivedData(const DataFrame &rxData);
signals:
    void dataSend(const DataFrame &txData);
    void receivedNewData(const DataFrame &rxData);
    void changedNodeID(quint16 id);
protected:
    virtual void processNewData(const DataFrame &rxData);
    bool processReceiveAcknowledged(const DataFrame &rxData);
private:
    quint16 m_range;
    quint16 m_node_id;
    qint16 m_layer_id;
    QPoint m_node_position;
    QWidget *m_Widget;
    bool m_connectedToWidget;
    QVector<NetworkNode*> m_connectedNodes;
    QVector<DataFrame> m_pendingSendDataFrames;
};

#endif // NETWORKNODE_H
