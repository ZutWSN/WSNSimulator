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
    void test_onReceivedDataFromCluster();
    void test_sendNewPaths();
};

#endif // TEST_SINKNODE_H