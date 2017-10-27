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

const QString TEST_CONFIG_FILE = "TestConfig.xml";

//add reading test config from file, saves compile time to setup which tests will be performed
namespace WSN_UnitTests_Config
{
struct Test
{
public:
    static bool readTestConfig()
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
                    std::cout <<  "\nStart Element :  " << xmlReader.name().toString().toStdString() << "\n";
                    if(!xmlReader.attributes().empty())
                    {
                        std::cout << xmlReader.attributes().
                    }
                }
                else if(xmlReader.isComment())
                {
                    std::cout << "\nComment : " << xmlReader.text().toString().toStdString() << "\n";
                }
                else if(xmlReader.isCharacters())
                {
                    std::cout << "\nCharacters : " << xmlReader.text().toString().toStdString() << "\n";
                }
                xmlReader.readNext();
            }
            readFileSuccessfully = true;
        }
        return readFileSuccessfully;
    }
private:
    void runNetworkNodesTest(bool report = false)
    {
        Test_NetworkNodes test_nodes;
        QStringList log;
        if(report)
        {
            log << "-o" << "NodesTest.txt,txt";
        }
        QTest::qExec(&test_nodes, log);
    }

    void runWidgetsTest(bool report = false)
    {
        Test_Widgets test_widgets;
        QStringList log;
        if(report)
        {
            log << "-o" << "WidgetsTest.txt,txt";
        }
        QTest::qExec(&test_widgets, log);
    }
};
}
#endif // TESTCONFIG_H
