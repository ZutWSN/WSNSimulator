#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QWidget>
#include <QLabel>

class DragWidget : public QLabel
{
    Q_OBJECT
public:
    DragWidget(QWidget *parent = 0);
    virtual ~DragWidget();

    enum DragWidgetType
    {
        Sensor = 0,
        Cluster = 1
    };
};

#endif // DRAGWIDGET_H
