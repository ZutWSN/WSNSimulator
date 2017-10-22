#ifndef SINKWIDGET_H
#define SINKWIDGET_H
#include "DragWidget.h"

class SinkWidget : public DragWidget
{
    Q_OBJECT
public:
    SinkWidget(QWidget *parent = 0, bool rootWidget = false);

    DragWidget::DragWidgetType getWidgetType() const;
};

#endif // SINKWIDGET_H
