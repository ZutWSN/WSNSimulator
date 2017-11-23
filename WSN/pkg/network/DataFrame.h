#ifndef DATAFRAME_H
#define DATAFRAME_H
#include <QByteArray>
#include <QMetaType>

class DataFrame
{
public:
    enum RxData
    {
        NO_DATA = 0,
        NEW_DATA,
        RECEIVED_DATA,
        PATH_SYNC,
        SENSOR_BROADCAST,
        NEIGHBOUR_PATH_REQ,
        NEIGHBOUR_PATH,
        CLUSTER_PATH,
        SENSOR_RESPONSE
    };

    DataFrame();
    DataFrame(const QByteArray &msg,
              DataFrame::RxData type,
              quint16 destination_id,
              quint16 layer_id,
              quint16 sender_id);
    DataFrame(const DataFrame &other);
    DataFrame(DataFrame &&other);
    DataFrame& operator=(const DataFrame &other);
    DataFrame& operator=(DataFrame &&other);
    ~DataFrame();

    void setMsg(const QByteArray &msg);
    void setMsgType(DataFrame::RxData type);
    void setDestination(const QPair<quint16, quint16> &destination);
    void setSender(const QPair<quint16, quint16> &sender);
    bool setPath(const QVector<quint16> &path);

    QByteArray getMsg() const;
    DataFrame::RxData getMsgType() const;
    QPair<quint16, quint16> getDestination() const;
    QPair<quint16, quint16> getSender() const;
    QPair<quint16, quint16> getNextChainNode(quint16 currentNodeID) const;
    bool frameEmpty() const;
    bool isFinalDestination(const QPair<quint16, quint16> &nodeLayerAndID) const;

    // later add compress data, maybe parsing a hex msg not just saving and reading a string
private:
    QByteArray m_Msg;
    DataFrame::RxData m_Type;
    QPair<quint16, quint16> m_desination;
    QPair<quint16, quint16> m_sender;
    QVector<quint16> m_path;
};

Q_DECLARE_METATYPE(DataFrame)

#endif // DATAFRAME_H
