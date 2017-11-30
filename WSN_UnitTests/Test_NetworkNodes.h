#ifndef TEST_NETWORKNODES_H
#define TEST_NETWORKNODES_H
#include <QObject>
#include <QtTest/QtTest>

class Test_NetworkNodes : public QObject
{
    Q_OBJECT
private slots:
    //NetworkNode tests    
    void test_connectToNode();
    void test_disconnectedFromNode();
    void test_connectToNodeWidget();
    void test_sendData();
    void test_getDistanceFromNode();
    void test_getDistanceFromConnectedNode();
    void test_getNumOfPendingDataFrames();
    void test_checkIfInRange();
};

#endif // TEST_NETWORKNODES_H
