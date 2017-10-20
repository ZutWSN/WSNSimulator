#ifndef SENSORWIDGET_H
#define SENSORWIDGET_H
#include "dragwidget.h"

class SensorWidget : public DragWidget
{
    Q_OBJECT
public:
    SensorWidget(QWidget *parent = 0, bool rootWidget = false);
    ~SensorWidget();
};

#endif // SENSORWIDGET_H
