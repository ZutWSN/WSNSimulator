#ifndef SINKNODE_H
#define SINKNODE_H
#include <QObject>
#include <QPoint>
#include <QVector>
#include "NetworkNode.h"
#include "NetworkLayer.h"
#include "DataFrame.h"

class SinkNode : public QObject
{
    Q_OBJECT
public:
    struct MappedClusterNode
    {
        quint16 node_id;
        quint16 layer_id;
        QPoint position;
        QVector<quint16> sinkPath;
    }

    static SinkNode *getSinkInstance();

    bool addDirectCluster(NetworkNode *cluster);
    bool removeDirectCluster(NetworkNode *cluster);
    bool connectToNodeWidget(QWidget *widget);
    bool disconnectFromWidget();
    void sendNewPaths();

    void setPosition(const QPoint &pos);
    void setRange(const QPoint &range);

    void getNumOfInRangeCusters() const;
    void getNumOfConnectedLayers() const;
    DataFrame getLastMsg() const;
    bool checkIfHasCluster(NetworkNode *cluster) const;
private:
    SinkNode(){};
    SinkNode(const SinkNode&) = delete;
    SinkNode& operator=(const SinkNode&) = delete;
    static SinkNode *m_sinkInstance;
public slots:
    void onReceivedDataFromCluster(const DataFrame &data);
signals:
    void receivedData(const DataFrame &data); //to widget
    void sendData(const DataFrame &data);
private:
    void broadCastDataToClusters(const DataFrame &data);
    void calculateNetworkPaths();
    void updateClusterPath(const DataFrame &data);
private:
    QPoint m_position;
    quint16 m_range;
    //clusters with direct connection to sink
    QVector<NetworkNode*> m_inRangeClusters;
    QVector<MappedClusterNode> m_clusterPathMap;
    QVector<NetworkLayer*> m_layers;
    DataFrame m_lastMsg;
};

#endif // SINKNODE_H
