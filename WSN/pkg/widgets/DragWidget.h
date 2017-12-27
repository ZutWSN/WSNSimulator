#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H
#include <QWidget>
#include <QLabel>
#include "DataFrame.h"

class DragWidget : public QLabel
{
    Q_OBJECT
public:
    enum DragWidgetType
    {
        NoType  = 0,
        Sensor  = 1,
        Cluster = 2,
        Sink    = 3,
    };

    DragWidget(QWidget *parent = 0, bool rootWidget = false);
    virtual ~DragWidget();
    void setWidgetImage(const QString &imgName);
    void setAsRootWidget();
    bool connectToNode(quint16 node_id, quint16 layer_id, double range);

    bool isRootWidget() const;
    virtual DragWidget::DragWidgetType getWidgetType() const;
    QString getImageName() const;
    bool isConnectedToNode() const;
    quint16 getNodeID() const;
    quint16 getLayerID() const;
    double getNodeRange() const;
    void setConnectedToNode(bool connected);
public slots:
    void onNodeReceivedData(const DataFrame &data);
private:
    QString m_imgName;
    bool m_rootWidget;
    bool m_connectedToNode;
    quint16 m_node_id;
    quint16 m_layer_id;
    double m_range;

};

#endif // DRAGWIDGET_H
