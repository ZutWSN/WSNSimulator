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
        NEW_DATA = 1,
        RECEIVED_DATA = 2,
    };

    DataFrame();
    DataFrame(const QByteArray &msg,
              DataFrame::RxData type,
              quint16 destination_id,
              quint16 layer_id,
              quint16 sender_id,
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

    QByteArray getMsg() const;
    DataFrame::RxData getMsgType() const;
    QPair<quint16, quint16> getDestination() const;
    QPair<quint16, quint16> getSender() const;
    bool frameEmpty() const;

    // later add compress data, maybe parsing a hex msg not just saving and reading a string
private:
    QByteArray m_Msg;
    DataFrame::RxData m_Type;
    QPair<quint16, quint16> m_desination;
    QPair<quint16, quint16> m_sender;
};

Q_DECLARE_METATYPE(DataFrame)

#endif // DATAFRAME_H
