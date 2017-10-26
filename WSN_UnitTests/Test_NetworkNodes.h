#ifndef TEST_NETWORKNODES_H
#define TEST_NETWORKNODES_H
#include <QObject>
#include <QtTest/QtTest>

class Test_NetworkNodes : public QObject
{
    Q_OBJECT
private slots:
    //NetworkNode tests
    void test_sendData();
    void test_onReceivedData();
    void test_connectToNode();
    void test_connectToNodeWidget();
};

#endif // TEST_NETWORKNODES_H
