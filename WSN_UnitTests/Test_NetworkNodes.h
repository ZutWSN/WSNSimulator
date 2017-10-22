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
    void test_WidgetFactory();
    void cleanupTestCase();
};

#endif // TEST_NETWORKNODES_H
