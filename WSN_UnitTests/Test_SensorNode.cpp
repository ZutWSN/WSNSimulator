#include "Test_SensorNode.h"
#include "SensorNode.h"
#include "ClusterNode.h"

void Test_SensorNode::test_connectToCluster()
{
    SensorNode sensor(0);
    ClusterNode cluster(1);
    QCOMPARE(sensor.connectToCluster(&cluster), true);
}
