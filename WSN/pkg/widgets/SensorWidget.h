#ifndef SENSORWIDGET_H
#define SENSORWIDGET_H
#include "dragwidget.h"

class SensorWidget : public DragWidget
{
    Q_OBJECT
public:
    SensorWidget(QWidget *parent = 0, bool rootWidget = false);

    DragWidget::DragWidgetType getWidgetType() const;
private:
    void processReceivedData(const DataFrame &data);
    void processDataSend(const DataFrame &data);
};

#endif // SENSORWIDGET_H
