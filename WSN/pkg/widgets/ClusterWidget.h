#ifndef CLUSTERWIDGET_H
#define CLUSTERWIDGET_H
#include "dragwidget.h"

class ClusterWidget : public DragWidget
{
    Q_OBJECT
public:
    ClusterWidget(QWidget *parent = 0, bool rootWidget = false);

    DragWidget::DragWidgetType getWidgetType() const;
private:
    void processReceivedData(const DataFrame &data);
    void processDataSend(const DataFrame &data);
};

#endif // CLUSTERWIDGET_H
