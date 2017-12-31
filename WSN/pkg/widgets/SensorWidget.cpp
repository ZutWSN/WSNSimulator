#include "sensorwidget.h"

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
    switch(data.getMsgType())
    {

    }
}

void SensorWidget::processDataSend(const DataFrame &data)
{
    switch(data.getMsgType())
    {

    }
}


