#ifndef NETWORKNODE_H
#define NETWORKNODE_H
#include "DataFrame.h"
#include "DragWidget.h"
#include <QObject>
#include <QPoint>

class NetworkNode : public QObject
{
    Q_OBJECT
public:
    enum NodeType
    {
        NoType  =   0,
        Sensor =    1,
        Cluster =   2,
        Sink =      3
    };

    NetworkNode(quint16 node_id = 0);
    virtual ~NetworkNode();

    bool sendData(const DataFrame &txData);   
    bool connectToNode(NetworkNode *node);

    void setNodeID(quint16 node_id);
    void setLayer(qint16 layer_id);
    bool connectToNodeWidget(DragWidget *widget);
    void setNodePosition(const QPoint &position);

    quint16 getNodeID() const;
    qint16 getNodeLayer() const;
    QPoint getNodePostion() const;
    bool getSendDataReceived() const;
    virtual NetworkNode::NodeType getNodeType() const;
public slots:
    void onReceivedData(const DataFrame &rxData);
signals:
    void dataSend(const DataFrame &txData);
    void receivedNewData(const DataFrame &rxData);
private:
    virtual void processData(const DataFrame &rxData);
private:
    bool m_sendDataReceived;
    bool m_connectedToWidget;
    quint16 m_node_id;
    qint16 m_layer_id;
    QPoint m_node_position;
};

#endif // NETWORKNODE_H
