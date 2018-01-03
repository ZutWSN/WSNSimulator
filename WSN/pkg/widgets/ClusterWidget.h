#ifndef ClusterWidget_H
#define ClusterWidget_H
#include "DragWidget.h"

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

#endif // ClusterWidget_H
