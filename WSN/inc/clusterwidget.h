#ifndef CLUSTERWIDGET_H
#define CLUSTERWIDGET_H
#include "dragwidget.h"

class ClusterWidget : public DragWidget
{
public:
    ClusterWidget(const QString imgFile, QWidget *parent = 0);
    ~ClusterWidget();
};

#endif // CLUSTERWIDGET_H
