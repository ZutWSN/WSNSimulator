#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H
#include "dragwidget.h"

class WidgetFactory
{
public:
    WidgetFactory();
    DragWidget* getNewDragWidget(DragWidget::DragWidgetType type, QWidget *parent = 0, bool rootWidget = false);
};

#endif // WIDGETFACTORY_H
