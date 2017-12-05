#ifndef SINKNODE_H
#define SINKNODE_H
#include <QObject>
#include <QPoint>
#include <QVector>
#include "NetworkNode.h"
#include "NetworkLayer.h"
#include "DataFrame.h"
//maybe make a singleton??always just one sink for entire network
//make copy, move constructors and assignment operators private
class SinkNode : public QObject
{
    Q_OBJECT
public:
    static SinkNode *getSinkInstance(const QPoint &node_position, quint16 range);

    bool addDirectCluster(NetworkNode *cluster);
    bool removeDirectCluster(NetworkNode *cluster);
    void sendNewPaths();

    void setPosition(const QPoint &pos);
    void setRange(const QPoint &range);

    void getNumOfInRangeCusters() const;
    void getNumOfConnectedLayers() const;
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
    void processNewData(const DataFrame &data);
    void broadCastDataToClusters(const DataFrame &data);
    void calculateNetworkPaths();
private:
    QPoint m_position;
    quint16 m_range;
    //clusters with direct connection to sink
    QVector<NetworkNode*> m_inRangeClusters;
    QVector<QVector<quint16> > m_clusterPaths;
    QVector<NetworkLayer*> m_layers;
};

#endif // SINKNODE_H
