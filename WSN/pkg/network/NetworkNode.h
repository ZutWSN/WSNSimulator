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

    void setNodePosition(const QPoint &position);

    quint16 getNodeID() const;
    qint16 getNodeLayer() const;
    QPoint getNodePostion() const;
    bool getSendDataReceived() const;
    virtual NetworkNode::NodeType getNodeType() const;
    bool checkIfInRange(const QPoint &position) const;
    bool checkIfConnectedToNode(NetworkNode *node) const;
public slots:
    void onReceivedData(const DataFrame &rxData);
signals:
    void dataSend(const DataFrame &txData);
    void receivedNewData(const DataFrame &rxData);
    void changedNodeID(quint16 id);
protected:
    virtual void processData(const DataFrame &rxData);
private:
    quint16 m_range;
    quint16 m_node_id;
    qint16 m_layer_id;
    QPoint m_node_position;
    QWidget *m_Widget;
    bool m_sendDataReceived;
    bool m_connectedToWidget;
    QVector<NetworkNode*> m_connectedNodes;
};

#endif // NETWORKNODE_H
