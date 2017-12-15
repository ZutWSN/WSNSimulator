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
        quint16 pathLength;
        QVector<quint16> neighbourIDs;
        QVector<quint16> neighbourDistances;
        QVector<quint16> sinkPath;

        bool operator==(const MappedClusterNode &rhs) const
        {
            bool equal = (node_id == rhs.node_id);
            equal &= (layer_id == rhs.layer_id);
            return equal;
        }
    };

    struct Vertice
    {
        quint16 node_id;
        quint16 layer_id;
        quint16 sinkPathLength;
        QVector<quint16> sinkPath;
        QVector<QPair<quint16, quint16> > neighbours; // <id, distance>
        QVector<quint16> neighbourVerticesIndexes;
        QVector<quint16> neighbourVerticesDistances;
        bool isDirectVertex = false;
        bool operator==(const Vertice &other)
        {
            return (node_id == other.node_id);
        }
    };

    static SinkNode* getSinkInstance();

    bool addDirectCluster(NetworkNode *cluster);
    bool removeDirectCluster(NetworkNode *cluster);
    bool connectToNodeWidget(QWidget *widget);
    bool disconnectFromWidget();
    void sendNewPaths();

    void setPosition(const QPoint &pos);
    void setRange(quint16 range);

    quint16 getNumOfInRangeCusters() const;
    quint16 getNumOfConnectedLayers() const;
    DataFrame getLastMsg() const;
    bool checkIfHasDirectCluster(NetworkNode *cluster) const;
    bool checkIfHasMappedCluster(quint16 node_id, quint16 layer_id) const;
private:
    SinkNode(){};
    SinkNode(const SinkNode&) = delete;
    SinkNode& operator=(const SinkNode&) = delete;
    static SinkNode *m_sinkInstance;
public slots:
    void onReceivedDataFromCluster(const DataFrame &data);
signals:
    void receivedData(const DataFrame &data); //to widget
    void broadCastDataToClusters(const DataFrame &data);
private:
    bool calculateNetworkPaths(QByteArray &updateMsg);//TDO
    bool updateClusterPath(const DataFrame &data); //TDO
    quint16 checkIfHasMappedCluster(const MappedClusterNode &node) const;
    void removeNode(const QByteArray &msg); //TDO
    QVector<Vertice> createGraphAndFindPaths() const;
    bool updatePathsAndCreateSyncMsg(const QVector<Vertice> &vertices, QByteArray &updateMsg);

    void extractNodeData(NetworkNode* node, Vertice &vertice) const;
    void extractMappedNodeData(const MappedClusterNode &node, Vertice &vertice) const;
private:
    QPoint m_position;
    quint16 m_range;
    //clusters with direct connection to sink
    QVector<NetworkNode*> m_inRangeClusters;
    QVector<MappedClusterNode> m_clusterPathMap;
    QVector<NetworkLayer*> m_layers;
    DataFrame m_lastMsg;
    QWidget *m_Widget;
    bool m_connectedToWidget;
};

#endif // SINKNODE_H
