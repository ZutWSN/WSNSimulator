#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H
#include "DragWidget.h"

class WidgetFactory
{
public:
    WidgetFactory();
    DragWidget* getNewDragWidget(DragWidget::DragWidgetType type, QWidget *parent = 0, bool rootWidget = false) const;
};

#endif // WIDGETFACTORY_H
