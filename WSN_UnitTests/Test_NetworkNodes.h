#ifndef TEST_NETWORKNODES_H
#define TEST_NETWORKNODES_H
#include <QObject>
#include <QtTest/QtTest>

class Test_NetworkNodes : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    //network nodes tests
    void test_sendData();
    void test_onReceivedData();
    void test_addToLayer();
    void test_removeFromLayer();
    void test_connectToNode();

    //widget tests
    void test_WidgetFactory();

    void cleanupTestCase();
};

#endif // TEST_NETWORKNODES_H
