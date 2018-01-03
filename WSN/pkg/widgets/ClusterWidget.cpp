#include "ClusterWidget.h"

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
    DragWidget::processReceivedData(data);
}

void ClusterWidget::processDataSend(const DataFrame &data)
{
    DragWidget::processDataSend(data);
}

