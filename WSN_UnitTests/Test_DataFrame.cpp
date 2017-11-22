#include "Test_DataFrame.h"
#include "DataFrame.h"

void Test_DataFrame::test_setMsg()
{
    DataFrame frame;
    QByteArray msg("This is Sparta!!");
    frame.setMsg(msg);
    QCOMPARE(frame.getMsg(), msg);
}

void Test_DataFrame::test_setMsgType()
{
    DataFrame frame;
    frame.setMsgType(DataFrame::RxData::NEW_DATA);
    QCOMPARE(frame.getMsgType(), DataFrame::RxData::NEW_DATA);
    frame.setMsgType(DataFrame::RxData::RECEIVED_DATA);
    QCOMPARE(frame.getMsgType(), DataFrame::RxData::RECEIVED_DATA);
}

void Test_DataFrame::test_setDestination()
{
    DataFrame frame;
    frame.setDestination(0);
    QCOMPARE(frame.getDestination(), static_cast<quint16>(0));
}

void Test_DataFrame::test_frameEmpty()
{
    DataFrame frame;
    QCOMPARE(frame.frameEmpty(), true);
    QByteArray msg("Some data");
    frame.setMsg(msg);
    QCOMPARE(frame.frameEmpty(), false);
}

void Test_DataFrame::test_setPath()
{
    DataFrame frame;
    QVector<QPair<quint16, quint16> > path;
    QCOMPARE(frame.setPath(path), false);
    path.push_back(qMakePair(0, 0));
    path.push_back(qMakePair(1, 0));
    QCOMPARE(frame.setPath(path), true);
}

