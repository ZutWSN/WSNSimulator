#include "clusterwidget.h"

ClusterWidget::ClusterWidget(QWidget *parent, bool rootWidget):
    DragWidget(parent, rootWidget)
{

}

DragWidget::DragWidgetType ClusterWidget::getWidgetType() const
{
    return DragWidgetType::Cluster;
}

void ClusterWidget::processReceivedData(const DataFrame &data)
{
    switch(data.getMsgType())
    {

    }
}

void ClusterWidget::processDataSend(const DataFrame &data)
{
    switch(data.getMsgType())
    {

    }
}

