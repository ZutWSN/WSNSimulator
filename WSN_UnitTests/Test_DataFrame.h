#ifndef TEST_DATAFRAME_H
#define TEST_DATAFRAME_H
#include <QObject>
#include <QtTest/QtTest>

class Test_DataFrame : public QObject
{
    Q_OBJECT
public:
    void test_setMsg();
    void test_setMsgType();
    void test_setDestination();
    void test_frameEmpty();
};
#endif // TEST_DATAFRAME_H
