#ifndef DATAFRAME_H
#define DATAFRAME_H
#include <QByteArray>
#include <QMetaType>

class DataFrame
{
public:
    DataFrame();
    DataFrame(const DataFrame &other);
    ~DataFrame();

    enum RxData
    {
        NEW_DATA =   0,
        RECEIVED_DATA =   1
    };

    void setMsg(const QByteArray &msg);
    void setMsgType(DataFrame::RxData type);

    QByteArray getMsg() const;
    DataFrame::RxData getMsgType() const;

    bool frameEmpty() const;

    // later add compress data, maybe parsing a hex msg not just saving and reading a string
private:
    QByteArray m_Msg;
    DataFrame::RxData m_Type;
};

Q_DECLARE_METATYPE(DataFrame)

#endif // DATAFRAME_H
