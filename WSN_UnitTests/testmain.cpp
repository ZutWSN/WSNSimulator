#include "testconfig.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WSN_UnitTests_Config::Test test;
    test.readTestConfig();
    return 0;
}
