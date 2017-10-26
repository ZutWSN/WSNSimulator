#include "Test_NetworkNodes.h"
#include "Test_Widgets.h"
#include <iostream>
#include <QStringList>
#include <QString>
//add reading test config from file, saves compile time to setup which tests will be performed
inline void runNetworkNodesTest(bool report = false)
{
    Test_NetworkNodes test_nodes;
    QStringList log;
    if(report)
    {
        log << "-o" << "NodesTest.txt,txt";
    }
    QTest::qExec(&test_nodes, log);
}

inline void runWidgetsTest(bool report = false)
{
    Test_Widgets test_widgets;
    QStringList log;
    if(report)
    {
        log << "-o" << "WidgetsTest.txt,txt";
    }
    QTest::qExec(&test_widgets, log);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    runNetworkNodesTest();
    return 0;
}
