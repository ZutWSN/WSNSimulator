#include "SinkWidget.h"
#include "ClusterNode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SinkWidget::SinkWidget(QWidget *parent, bool rootWidget):
    DragWidget(parent, rootWidget)
{

}

DragWidget::DragWidgetType SinkWidget::getWidgetType() const
{
    return DragWidgetType::Sink;
}

void SinkWidget::processReceivedData(const DataFrame &data) const
{
    //for now use the old one
    DragWidget::processReceivedData(data);
}

void SinkWidget::processDataSend(const DataFrame &data) const
{
    QByteArray logMsg;
    switch(data.getMsgType())
    {
        case DataFrame::RxData::NO_SINK_CONNECTION:
            logMsg.append("No sink connection");
            break;
        case DataFrame::RxData::PATH_SYNC:
            logMsg.append("New paths send for mapped clusters: \n");
            getNewPathMsg(data.getMsg(), logMsg);
            break;
        default:
            break;
    }
    if(!logMsg.isEmpty())
    {
        emit sinkSendData(logMsg);
    }
}

void SinkWidget::getNewPathMsg(const QByteArray &serializedMsg, QByteArray &pathMsg) const
{
    QJsonDocument jsonData = QJsonDocument::fromBinaryData(serializedMsg, QJsonDocument::Validate);
    if(!jsonData.isNull())
    {
        QJsonObject jsonObj = jsonData.object();
        auto keys = jsonObj.keys();
        for(auto && key : keys)
        {
            if(key == PATHS)
            {
                pathMsg.append(key + "\n");
                if(jsonObj[key].isArray())
                {
                    auto listOfPaths = jsonObj[key].toArray();
                    for(auto && path : listOfPaths)
                    {
                        if(path.isObject())
                        {
                            auto pathObj = path.toObject();
                            auto pathKeys = pathObj.keys();
                            for(auto && pKey : pathKeys)
                            {
                                if(pKey == PATH)
                                {
                                    pathMsg.append("\n" + pKey + ":\n");
                                    for(auto && id: pathObj[pKey].toArray())
                                    {
                                        if(id.isDouble())
                                        {
                                            pathMsg.append(id.toString() + " ");
                                        }
                                    }
                                }
                                else
                                {
                                    pathMsg.append(pKey + pathObj[pKey].toString() + "\n");
                                }
                            }
                        }

                    }
                }
            }
        }
    }
}

