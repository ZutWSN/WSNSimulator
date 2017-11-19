#ifndef TEST_NETWORKLAYER_H
#define TEST_NETWORKLAYER_H
#include <QObject>
#include <QtTest/QtTest>

class Test_NetworkLayer : public QObject
{
    Q_OBJECT
private slots:
    void test_createNode();
    void test_connectNodes();
    void test_connectNodeWidget();
    void test_removeNode();
    void test_setLayerId();
};

#endif // TEST_NETWORKLAYER_H
