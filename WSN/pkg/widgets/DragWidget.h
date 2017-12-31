#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H
#include <QWidget>
#include <QLabel>
#include "DataFrame.h"

const QString DRAG_WIDGET_LABEL = "dragWidget";
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
    void connectToNode(quint16 node_id, quint16 layer_id, double range);

    void setConnectedToNode(bool connected);
    void setNodeID(quint16 node_id);
    void setRange(double range);
    void setNodeLayer(quint16 layer_id);

    bool isRootWidget() const;
    virtual DragWidget::DragWidgetType getWidgetType() const;
    QString getImageName() const;
    bool isConnectedToNode() const;
    quint16 getNodeID() const;
    quint16 getLayerID() const;
    double getNodeRange() const;
    QPoint getPosition() const;
protected:
    virtual void processReceivedData(const DataFrame &data) const;
    virtual void processDataSend(const DataFrame &data) const;
public slots:
    void onNodeReceivedData(const DataFrame &data);
    void onNodeSendData(const DataFrame &data);
signals:
    void sendWidgetReceivedData(const QByteArray &data, quint16 node_id, quint16 layer_id) const;
    void sendWidgetSendData(const QByteArray &data, quint16 node_id, quint16 layer_id) const;
protected:
    QString m_imgName;
    bool m_rootWidget;
    bool m_connectedToNode;
    QSize m_widgetSize;
    quint16 m_node_id;
    quint16 m_layer_id;
    double m_range;

};

#endif // DRAGWIDGET_H
