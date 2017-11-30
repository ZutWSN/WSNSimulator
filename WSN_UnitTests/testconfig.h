#ifndef TESTCONFIG_H
#define TESTCONFIG_H
#include <QString>
#include <functional>

static const QString TEST_CONFIG_FILE       = "TestConfig.xml";
static const QString TEST_NODE              = "Test";
static const QString TEST_NODE_ATTRIBUTE    = "enabled";
static const QString TEST_SAVE_REPORT_NODE  = "saveReport";
static const QString TEST_NAME_NODE         = "name";
static const QString TEST_REPORTFILE_NODE   = "reportFile";

namespace WSN_UnitTests_Config
{
    class TestConfig
    {
    public:
        bool readTestConfig();

    private:
        void runNetworkNodesTest(const QString &reportFile, bool report);
        void runNetworkLayerTest(const QString &reportFile, bool report);
        void runSensorNodeTest(const QString &reportFile, bool report);
        void runSensorNetworkTest(const QString &reportFile, bool report);
        void runDataFrameTest(const QString &reportFile, bool report);
        void runWidgetsTest(const QString &reportFile, bool report);

        bool getTestFunction(std::function<void(const QString&, bool)> &testFunction, const QString &testName);
    };
}
#endif // TESTCONFIG_H
