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
        double pathLength;
        QVector<quint16> neighbourIDs;
        QVector<double> neighbourDistances;
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
        double sinkPathLength;
        QVector<quint16> sinkPath;
        QVector<QPair<quint16, double> > neighbours; // <id, distance>
        QVector<quint16> neighbourVerticesIndexes;
        QVector<double> neighbourVerticesDistances;
        bool isDirectVertex = false;
        bool operator==(const Vertice &other)
        {
            return (node_id == other.node_id);
        }
    };
    SinkNode();
    SinkNode(const QPoint &position, double range);

    bool addDirectCluster(NetworkNode *cluster);
    bool removeDirectCluster(NetworkNode *cluster);
    bool connectToNodeWidget(QWidget *widget);
    bool disconnectFromWidget();
    void disconnectSinkFromNetwork();
    void sendNewPaths(quint16 senderLayer);

    void setPosition(const QPoint &pos);
    void setRange(quint16 range);

    quint16 getNumOfInRangeCusters() const;
    quint16 getNumOfConnectedLayers() const;
    DataFrame getLastMsg() const;
    QPoint getSinkPosition() const;
    double getSinkRange() const;
    bool checkIfHasDirectCluster(NetworkNode *cluster) const;
    bool checkIfHasMappedCluster(quint16 node_id, quint16 layer_id) const;
private:
    //makes class not copyable
    SinkNode(const SinkNode&) = delete;
    SinkNode& operator=(const SinkNode&) = delete;
public slots:
    void onReceivedDataFromCluster(const DataFrame &data);
signals:
    void receivedData(const DataFrame &data); //to widget
    void broadCastDataToClusters(const DataFrame &data);
private:
    bool calculateNetworkPaths(QByteArray &updateMsg);
    bool updateClusterPath(const DataFrame &data);
    quint16 checkIfHasMappedCluster(const MappedClusterNode &node) const;
    void removeNode(const QByteArray &msg);
    QVector<Vertice> createGraphAndFindPaths() const;
    bool updatePathsAndCreateSyncMsg(const QVector<Vertice> &vertices, QByteArray &updateMsg);
    void resetMappedSinkPaths();
    void extractNodeData(NetworkNode* node, Vertice &vertice) const;
    void extractMappedNodeData(const MappedClusterNode &node, Vertice &vertice) const;
private:
    QPoint m_position;
    double m_range;
    //clusters with direct connection to sink
    QVector<NetworkNode*> m_inRangeClusters;
    QVector<MappedClusterNode> m_clusterPathMap;
    QVector<NetworkLayer*> m_layers;
    DataFrame m_lastMsg;
    QWidget *m_Widget;
    bool m_connectedToWidget;
};

#endif // SINKNODE_H
