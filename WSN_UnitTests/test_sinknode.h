#ifndef TEST_SINKNODE_H
#define TEST_SINKNODE_H
#include <QObject>
#include <QtTest/QtTest>

class Test_SinkNode : public QObject
{
    Q_OBJECT
private slots:
    void test_getSinkInstance();
    void test_addRemoveDirectCluster();
    void test_receivedDataFromCluster();
    //TDO
    void test_receivedNodeRemoved();
    void test_receivedClusterPath();
    void test_sendNewPaths();
};

#endif // TEST_SINKNODE_H
