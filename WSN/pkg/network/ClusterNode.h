#ifndef CLUSTERNODE_H
#define CLUSTERNODE_H
#include "NetworkNode.h"
#include "sinknode.h"
#include <QString>

/*Adding new cluster :
 *
 * 1.Connect to all neighbour clusters and if in range sink.
 * 2.If not direct connection to sink - not in range:
 * - create best path to sink according to rule:
 *      R_best = R.indexOf(min(Ri + di))
 *      i = 0 ... N - 1; where N is number of connected neighbour clusters
 *      R - list of all neighbour path lengths
 * - add itself as last element and send path to sink
 * 3. Until the sink does not have all paths stored no communication is allowed inside the network betweem the
 * clusters, only reads from sensors, which will be stored duriing this time
*/
//move this to some public header like a global network nodes configuration file
const QString LAYER_ID              = "Layer_ID";
const QString NODE_ID               = "Node_ID";
const QString NODE_POSITION_X       = "Position_X";
const QString NODE_POSITION_Y       = "Position_Y";
const QString NODE_STATE            = "Node_State";
const QString PATH                  = "Path";
const QString PATH_LENGTH           = "Path_Length";
const QString NEIGHBOURS_IDS        = "NeighbourIDs";
const QString NEIGHBOURS_DISTANCES  = "NeighbourDistances";
const QString PATHS                 = "Paths";

const quint8 NUM_OF_PATH_MESSAGE_PARAMS = 8;
const quint8 NUM_OF_REMOVE_MESSAGE_PARAMS = 5;

class ClusterNode : public NetworkNode
{
    Q_OBJECT
public:
    enum ClusterStates
    {
        CREATED = 0,
        PATH_SEEKING,
        CONNECTED,
        CONNECTED_TO_SINK,
        DISCONNECTED
    };

    ClusterNode(quint16 node_id);
    ClusterNode(quint16 node_id, quint16 range, qint16 layer_id, const QPoint node_position);
    ~ClusterNode();

    bool operator==(const ClusterNode &rhs) const
    {
        bool equal = (getNodeID() == rhs.getNodeID());
        equal &= (getNodeLayer() == rhs.getNodeLayer());
        equal &= (getNodePostion() == rhs.getNodePostion());
        return equal;
    }

    bool connectToNode(NetworkNode *node);
    bool sendSinkPathReq();
    bool disconnectFromNode(NetworkNode *node);
    bool disconnectFromNetwork();
    bool addNode(NetworkNode *node);
    bool removeSensor(NetworkNode *sensor);
    //those two methods will be deleted after completed testing
    bool setSinkPath(const QVector<quint16> &path);
    void setPathLength(quint16 length);
    bool setConnectedToSink(SinkNode *sink);

    quint16 getNumOfSensors() const;
    quint16 getNumOfSensorPendingMsgs() const;
    ClusterStates getCurrentState() const;
    QVector<quint16> getSinkPath() const;

    bool checkIfConnectedToSensor(NetworkNode *sensor) const;
    NetworkNode::NodeType getNodeType() const;
public slots:
    void onReceivedDataFromSensor(const QByteArray &data);
signals:
    void broadcastDataToSensors(const DataFrame &data);
    void sendDataToSink(const DataFrame &data);
private:
    quint16 getPathFromNeighbourMsg(const DataFrame &rxData, QVector<quint16> &path);
    void processNewData(const DataFrame &rxData);
    bool extractPathFromMsg(const QByteArray &pathMsg);
    bool createClusterPathMsg(const QVector<quint16> &path, QByteArray &msg);
private:
    QVector<quint16> m_sinkPath;
    QVector<NetworkNode*> m_sensors;
    QByteArray m_mesgData;
    quint16 m_sensorDataCounter;
    quint16 m_neighbourPathsCounter;
    quint16 m_pathLength;
    ClusterStates m_state;
};

#endif // CLUSTERNODE_H
