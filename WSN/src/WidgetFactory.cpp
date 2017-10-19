#include "widgetfactory.h"
#include "sensorwidget.h"
#include "clusterwidget.h"

WidgetFactory::WidgetFactory()
{

}

WidgetFactory::WidgetFactory(const WidgetFactory &other)
{

}

WidgetFactory &WidgetFactory::operator=(const WidgetFactory &other)
{

}

DragWidget *WidgetFactory::getNewDragWidget(DragWidget::DragWidgetType type, QWidget *parent)
{
    DragWidget *newWidget = nullptr;
    switch(type)
    {
    case DragWidget::DragWidgetType::Sensor :
        newWidget = new SensorWidget(parent);
        break;
    case DragWidget::DragWidgetType::Cluster :
        newWidget = new ClusterWidget(parent);
        break;
    }

    return newWidget;
}
