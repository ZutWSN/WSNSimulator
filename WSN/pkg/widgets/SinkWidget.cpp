#include "SinkWidget.h"

SinkWidget::SinkWidget(QWidget *parent, bool rootWidget):
    DragWidget(parent, rootWidget)
{

}

DragWidget::DragWidgetType SinkWidget::getWidgetType() const
{
    return DragWidgetType::Sink;
}

