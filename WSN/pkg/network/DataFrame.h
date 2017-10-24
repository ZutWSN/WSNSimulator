#ifndef DATAFRAME_H
#define DATAFRAME_H
#include <QByteArray>
#include <QMetaType>

class DataFrame
{
public:
    enum RxData
    {
        NEW_DATA = 0,
        RECEIVED_DATA = 1
    };

    DataFrame();
    DataFrame(const QByteArray &msg, DataFrame::RxData type, quint16 destination_id);
    DataFrame(const DataFrame &other);
    ~DataFrame();

    void setMsg(const QByteArray &msg);
    void setMsgType(DataFrame::RxData type);
    void setDestination(quint16 node_id);

    QByteArray getMsg() const;
    DataFrame::RxData getMsgType() const;
    quint16 getDestination() const;

    bool frameEmpty() const;

    // later add compress data, maybe parsing a hex msg not just saving and reading a string
private:
    QByteArray m_Msg;
    DataFrame::RxData m_Type;
    quint16 m_desination_id;
};

Q_DECLARE_METATYPE(DataFrame)

#endif // DATAFRAME_H
