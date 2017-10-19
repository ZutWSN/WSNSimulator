#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H
#include "dragwidget.h"

class WidgetFactory
{
public:
    WidgetFactory();
    WidgetFactory(const WidgetFactory &other);
    WidgetFactory &operator=(const WidgetFactory &other);

    DragWidget* getNewDragWidget(DragWidget::DragWidgetType type, QWidget *parent = 0);
};

#endif // WIDGETFACTORY_H
