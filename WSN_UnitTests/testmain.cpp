#include "Test_NetworkNodes.h"

int main(int argc, char *argv[])
{
    Test_NetworkNodes test;
    QTest::qExec(&test);
    return 0;
}