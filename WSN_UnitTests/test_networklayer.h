#ifndef TEST_NETWORKLAYER_H
#define TEST_NETWORKLAYER_H
#include <QObject>
#include <QtTest/QtTest>

class Test_NetworkLayer : public QObject
{
    Q_OBJECT
public:
    void test_addToLayer();
    void test_removeFromLayer();
};

#endif // TEST_NETWORKLAYER_H
