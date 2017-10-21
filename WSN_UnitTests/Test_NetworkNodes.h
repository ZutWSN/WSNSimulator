#ifndef TEST_NETWORKNODES_H
#define TEST_NETWORKNODES_H
#include <QObject>
#include <QtTest/QtTest>

class Test_NetworkNodes : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void test_sendData();
    void test_connectToNode();
    void cleanupTestCase();
};

#endif // TEST_NETWORKNODES_H
