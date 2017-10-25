#include "Test_NetworkNodes.h"
#include "Test_Widgets.h"
#include <iostream>
#include <QStringList>
#include <QString>

int main(int argc, char *argv[])
{
    Test_NetworkNodes test_nodes;
    QStringList log;
    QApplication a(argc, argv);
    log << "-o" << "TestNodes_report.txt";
    QTest::qExec(&test_nodes, a.arguments());
    Test_Widgets test_widgets;
    log.clear();
    log << "-o" << "WidgetsTest.txt,txt";
    QTest::qExec(&test_widgets, log);
    return 0;
}
