#ifndef TEST_WIDGETS_H
#define TEST_WIDGETS_H
#include <QObject>
#include <QtTest/QtTest>

class Test_Widgets : public QObject
{
    Q_OBJECT
private slots:
    //widget tests
    void test_WidgetFactory();
};

#endif // TEST_WIDGETS_H
