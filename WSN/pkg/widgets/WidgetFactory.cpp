#include "widgetfactory.h"
#include "SensorWidget.h"
#include "ClusterWidget.h"
#include "SinkWidget.h"

WidgetFactory::WidgetFactory()
{

}

DragWidget *WidgetFactory::getNewDragWidget(DragWidget::DragWidgetType type, QWidget *parent, bool rootWidget) const
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
        case DragWidget::DragWidgetType::Sink :
            newWidget = new SinkWidget(parent, rootWidget);
            break;
        default:
            break;
    }

    return newWidget;
}
