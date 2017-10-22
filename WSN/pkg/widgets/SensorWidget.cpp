#include "sensorwidget.h"

SensorWidget::SensorWidget(QWidget *parent, bool rootWidget):
    DragWidget(parent, rootWidget)
{

}

DragWidget::DragWidgetType SensorWidget::getWidgetType() const
{
    return DragWidgetType::Sensor;
}


