#ifndef CLUSTERWIDGET_H
#define CLUSTERWIDGET_H
#include "dragwidget.h"

class ClusterWidget : public DragWidget
{
    Q_OBJECT
public:
    ClusterWidget( QWidget *parent = 0, bool rootWidget = false);
    ~ClusterWidget();
};

#endif // CLUSTERWIDGET_H
