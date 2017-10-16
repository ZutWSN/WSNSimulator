#ifndef SENSORWIDGET_H
#define SENSORWIDGET_H
#include "dragwidget.h"

class SensorWidget : public DragWidget
{
public:
    SensorWidget(const QString imgFile, QWidget *parent = 0);
    ~SensorWidget();
};

#endif // SENSORWIDGET_H
