#ifndef TESTCONFIG_H
#define TESTCONFIG_H
#include <QStringList>
#include <QString>
//test objetcs includes
#include "test_networklayer.h"
#include "Test_Widgets.h"
#include "Test_SensorNetwork.h"
#include "Test_NetworkNodes.h"
#include "Test_DataFrame.h"
#include <iostream>
#include <functional>

using namespace std::placeholders;

static const QString TEST_CONFIG_FILE = "TestConfig.xml";
static const QString TEST_NODE = "Test";
static const QString TEST_NODE_ATTRIBUTE = "enabled";
static const QString TEST_SAVE_REPORT_NODE = "saveReport";
static const QString TEST_NAME_NODE = "name";
static const QString TEST_REPORTFILE_NODE = "reportFile";

namespace WSN_UnitTests_Config
{
struct Test
{
public:
    bool readTestConfig()
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
                }
                xmlReader.readNext();
            }
            readFileSuccessfully = true;
        }
        return readFileSuccessfully;
    }
private:
    void runNetworkNodesTest(const QString &reportFile, bool report)
    {
        Test_NetworkNodes test_nodes;
        QStringList log;
        if(report)
        {
            log << "-o" << reportFile;
        }
        QTest::qExec(&test_nodes, log);
    }

    void runWidgetsTest(const QString &reportFile, bool report)
    {
        Test_Widgets test_widgets;
        QStringList log;
        if(report)
        {
            log << "-o" << reportFile;
        }
        QTest::qExec(&test_widgets, log);
    }

    bool getTestFunction(std::function<void(const QString&, bool)> &testFunction, const QString &testName)
    {
        bool validTestName = true;
        if(testName == "NetworkNodesTest")
        {
            testFunction = std::bind(&WSN_UnitTests_Config::Test::runNetworkNodesTest, this, _1, _2);
        }
        else if(testName == "WidgetsTest")
        {
            testFunction = std::bind(&WSN_UnitTests_Config::Test::runWidgetsTest, this, _1, _2);
        }
        else
        {
            validTestName = false;
        }
        return validTestName;
    }
};
}
#endif // TESTCONFIG_H
