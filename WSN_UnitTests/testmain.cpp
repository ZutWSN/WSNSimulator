#include "testconfig.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WSN_UnitTests_Config::Test::readTestConfig();
    return 0;
}
