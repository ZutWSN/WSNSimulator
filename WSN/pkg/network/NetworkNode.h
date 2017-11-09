#ifndef NETWORKNODE_H
#define NETWORKNODE_H
#include "DataFrame.h"
#include <QObject>
#include <QVector>
#include <QPoint>

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
    virtual ~NetworkNode();

    bool sendData(const DataFrame &txData);   
    virtual bool connectToNode(NetworkNode *node) = 0;
    virtual bool disconnectFromNode(NetworkNode *node) = 0;

    void setNodeID(quint16 node_id);
    void setLayer(qint16 layer_id);
    bool connectToNodeWidget(QWidget *widget);
    void setNodePosition(const QPoint &position);

    quint16 getNodeID() const;
    qint16 getNodeLayer() const;
    QPoint getNodePostion() const;
    bool getSendDataReceived() const;
    virtual NetworkNode::NodeType getNodeType() const;
    bool checkIfInRange(QPoint &position) const;
public slots:
    void onReceivedData(const DataFrame &rxData);
signals:
    void dataSend(const DataFrame &txData);
    void receivedNewData(const DataFrame &rxData);
    void changedNodeID(quint16 id);
private:
    virtual void processData(const DataFrame &rxData);
private:
    bool m_sendDataReceived;
    bool m_connectedToWidget;
    quint16 m_range;
    quint16 m_node_id;
    qint16 m_layer_id;
    QPoint m_node_position;
    QWidget *m_Widget;
};

#endif // NETWORKNODE_H
