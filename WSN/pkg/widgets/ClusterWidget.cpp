#include "clusterwidget.h"

ClusterWidget::ClusterWidget(QWidget *parent, bool rootWidget):
    DragWidget(parent, rootWidget)
{

}

DragWidget::DragWidgetType ClusterWidget::getWidgetType() const
{
    return DragWidgetType::Cluster;
}

