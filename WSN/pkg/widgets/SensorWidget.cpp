#include "SensorWidget.h"

SensorWidget::SensorWidget(QWidget *parent, bool rootWidget):
    DragWidget(parent, rootWidget)
{

}

DragWidget::DragWidgetType SensorWidget::getWidgetType() const
{
    return DragWidgetType::Sensor;
}

void SensorWidget::processReceivedData(const DataFrame &data)
{
    DragWidget::processReceivedData(data);
}

void SensorWidget::processDataSend(const DataFrame &data)
{
    DragWidget::processDataSend(data);
}


