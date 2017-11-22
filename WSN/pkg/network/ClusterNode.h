#ifndef CLUSTERNODE_H
#define CLUSTERNODE_H
#include "NetworkNode.h"
#include <QString>

/*Adding new cluster :
 *
 * 1.Connect to all neighbour clusters and if in range sink.
 * 2.If not direct connection to sink - not in range:
 * - create best path to sink according to rule:
 *      R_best = R.indexOf(min(Ri + di))
 *      i = 0 ... N - 1; where N is number of connected neighbour clusters
 * - add itself as last element and send path to sink
 * 3. Until the sink does not have all paths stored no communication is allowed inside the network betweem the
 * clusters, only reads from sensors, which will be stored duriing this time

class ClusterNode : public NetworkNode
{
    Q_OBJECT
public:
    ClusterNode(quint16 node_id);
    ClusterNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position);
    ~ClusterNode();

    bool connectToNode(NetworkNode *node);
    bool disconnectFromNode(NetworkNode *node);

    quint16 getNumOfSensors() const;
    bool checkIfConnectedToSensor(NetworkNode *sensor) const;
    NetworkNode::NodeType getNodeType() const;
signals:
    void broacastDataToSensors(const DataFrame &rxData);
private:
    void processNewData(const DataFrame &rxData);
    bool extractNewPathFromMsg(const QByteArray &pathMsg);
private:
    QVector<QPair<quint16, quint16> > m_sinkPath;
    QVector<NetworkNode*> m_sensors;
};

#endif // CLUSTERNODE_H
