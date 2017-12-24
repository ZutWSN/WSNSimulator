#ifndef TEST_SINKNODE_H
#define TEST_SINKNODE_H
#include <QObject>
#include <QtTest/QtTest>

class Test_SinkNode : public QObject
{
    Q_OBJECT
private slots:
    void test_addRemoveDirectCluster();
    void test_receivedDataFromCluster();
    void test_onReceivedNetworkChange();
    //TDO
    void test_disconnectSinkFromNetwork();
};

#endif // TEST_SINKNODE_H
