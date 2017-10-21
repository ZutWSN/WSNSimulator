#include "widgetfactory.h"
#include "sensorwidget.h"
#include "clusterwidget.h"

WidgetFactory::WidgetFactory()
{

}

DragWidget *WidgetFactory::getNewDragWidget(DragWidget::DragWidgetType type, QWidget *parent, bool rootWidget)
{
    DragWidget *newWidget = nullptr;
    switch(type)
    {
    case DragWidget::DragWidgetType::Sensor :
        newWidget = new SensorWidget(parent, rootWidget);
        break;
    case DragWidget::DragWidgetType::Cluster :
        newWidget = new ClusterWidget(parent, rootWidget);
        break;
    }

    return newWidget;
}
