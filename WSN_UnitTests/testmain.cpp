#include "testconfig.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WSN_UnitTests_Config::TestConfig test;
    test.readTestConfig();
    return 0;
}
