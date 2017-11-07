#include "testconfig.h"
#include <QStringList>
//test objetcs includes
#include "test_networklayer.h"
#include "Test_Widgets.h"
#include "Test_SensorNetwork.h"
#include "Test_NetworkNodes.h"
#include "Test_DataFrame.h"
#include "Test_SensorNode.h"
#include <functional>

using namespace std::placeholders;

bool WSN_UnitTests_Config::TestConfig::readTestConfig()
{
    bool readFileSuccessfully = false;
    QFile config(TEST_CONFIG_FILE);
    if(config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader xmlReader(&config);
        while(!xmlReader.isEndDocument())
        {
            if(xmlReader.isStartElement())
            {
                if(xmlReader.name().toString() == TEST_NODE)
                {
                    for(auto && attr : xmlReader.attributes())
                    {
                        if(attr.name().toString() == TEST_NODE_ATTRIBUTE)
                        {
                            if(attr.value().toString().toStdString() == "true")
                            {
                                std::function<void(const QString&, bool)> testFunction;
                                bool saveReport = false;
                                bool testCaseFound = false;
                                QString reportFile;
                                while(!(xmlReader.isEndElement() && xmlReader.name() == TEST_NODE))
                                {
                                    xmlReader.readNext();
                                    if(xmlReader.isStartElement() && (xmlReader.name().toString() == TEST_NAME_NODE))
                                    {
                                        xmlReader.readNext();
                                        if(xmlReader.isCharacters())
                                        {
                                            testCaseFound = getTestFunction(testFunction, xmlReader.text().toString());
                                        }
                                    }
                                    if(xmlReader.isStartElement() && (xmlReader.name().toString() == TEST_SAVE_REPORT_NODE))
                                    {
                                        xmlReader.readNext();
                                        if(xmlReader.isCharacters())
                                        {
                                            saveReport = (xmlReader.text().toString() == "true") ? true : false;
                                        }
                                    }
                                    if(xmlReader.isStartElement() && (xmlReader.name().toString() == TEST_REPORTFILE_NODE))
                                    {
                                        xmlReader.readNext();
                                        if(xmlReader.isCharacters())
                                        {
                                            reportFile = xmlReader.text().toString();
                                        }
                                    }
                                }
                                //call test function
                                if(testCaseFound)
                                {
                                    testFunction(reportFile, saveReport);
                                    testCaseFound = false;
                                }
                            }
                        }
                    }
                }
            }
            xmlReader.readNext();
        }
        readFileSuccessfully = true;
    }
    return readFileSuccessfully;
 }
void WSN_UnitTests_Config::TestConfig::runNetworkNodesTest(const QString &reportFile, bool report)
{
    Test_NetworkNodes test_nodes;
    QStringList log;
    if(report)
    {
        log << "." << "-o" << reportFile;
    }
    QTest::qExec(&test_nodes, log);
}

void WSN_UnitTests_Config::TestConfig::runNetworkLayerTest(const QString &reportFile, bool report)
{
    Test_NetworkLayer test_layer;
    QStringList log;
    if(report)
    {
        log << "." << "-o" << reportFile;
    }
    QTest::qExec(&test_layer, log);
}

void WSN_UnitTests_Config::TestConfig::runSensorNetworkTest(const QString &reportFile, bool report)
{
    Test_SensorNetwork test_network;
    QStringList log;
    if(report)
    {
        log << "." << "-o" << reportFile;
    }
    QTest::qExec(&test_network, log);
}

void WSN_UnitTests_Config::TestConfig::runDataFrameTest(const QString &reportFile, bool report)
{
    Test_DataFrame test_frame;
    QStringList log;
    if(report)
    {
        log << "." << "-o" << reportFile;
    }
    QTest::qExec(&test_frame, log);
}

void WSN_UnitTests_Config::TestConfig::runWidgetsTest(const QString &reportFile, bool report)
{
    Test_Widgets test_widgets;
    QStringList log;
    if(report)
    {
        log << "." <<"-o" << reportFile;
    }
    QTest::qExec(&test_widgets, log);
}

void WSN_UnitTests_Config::TestConfig::runSensorNodeTest(const QString &reportFile, bool report)
{
    Test_SensorNode test_sensor;
    QStringList log;
    if(report)
    {
        log << "." <<"-o" << reportFile;
    }
    QTest::qExec(&test_sensor, log);
}

bool WSN_UnitTests_Config::TestConfig::getTestFunction(std::function<void(const QString&, bool)> &testFunction, const QString &testName)
{
    bool validTestName = true;
    if(testName == "NetworkNodesTest")
    {
        testFunction = std::bind(&WSN_UnitTests_Config::TestConfig::runNetworkNodesTest, this, _1, _2);
    }
    else if(testName == "WidgetsTest")
    {
        testFunction = std::bind(&WSN_UnitTests_Config::TestConfig::runWidgetsTest, this, _1, _2);
    }
    else if(testName == "NetworkLayerTest")
    {
        testFunction = std::bind(&WSN_UnitTests_Config::TestConfig::runNetworkLayerTest, this, _1, _2);
    }
    else if(testName == "SensorNetworkTest")
    {
        testFunction = std::bind(&WSN_UnitTests_Config::TestConfig::runSensorNetworkTest, this, _1, _2);
    }
    else if(testName == "DataFrameTest")
    {
        testFunction = std::bind(&WSN_UnitTests_Config::TestConfig::runDataFrameTest, this, _1, _2);
    }
    else if(testName == "SensorNodeTest")
    {
        testFunction = std::bind(&WSN_UnitTests_Config::TestConfig::runSensorNodeTest, this, _1, _2);
    }
    else
    {
        validTestName = false;
    }
    return validTestName;
}

