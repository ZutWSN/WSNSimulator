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
    QPair<quint16, quint16> destination(0, 0);
    frame.setDestination(destination);
    QVERIFY(frame.getDestination() == destination);
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
    QVector<quint16> path;
    QCOMPARE(frame.setPath(path), false);
    path.push_back(0);
    path.push_back(1);
    QCOMPARE(frame.setPath(path), true);
}

