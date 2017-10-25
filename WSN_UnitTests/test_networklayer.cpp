#include "test_NetworkLayer.h"
#include "NetworkLayer.h"
#include "NetworkNode.h"

void Test_NetworkLayer::test_addToLayer()
{
    NetworkNode sender(0);
    NetworkNode receiver(1);
    NetworkLayer layer(0);
    layer.addNode(&sender);
    layer.addNode(&receiver);
    QCOMPARE(sender.getNodeLayer(), layer.getLayerId());
    QCOMPARE(receiver.getNodeLayer(), layer.getLayerId());
}

void Test_NetworkLayer::test_removeFromLayer()
{
    NetworkNode sender(0);
    NetworkNode receiver(1);
    NetworkLayer layer(0);
    layer.addNode(&sender);
    layer.addNode(&receiver);
    layer.removeNode(&sender);
    layer.removeNode(&receiver);
    QCOMPARE(sender.getNodeLayer() == layer.getLayerId(), false);
    QCOMPARE(receiver.getNodeLayer() == layer.getLayerId(), false);
}
