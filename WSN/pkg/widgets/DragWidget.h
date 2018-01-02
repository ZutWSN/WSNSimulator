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
        NoType = 0,
        Sensor,
        Cluster,
        Sink
    };

    enum ImageType
    {
        DEFAULT = 0,
        RECEIVED_DATA,
        SEND_DATA
    };

    DragWidget(QWidget *parent = 0, bool rootWidget = false);
    virtual ~DragWidget();
    void setAsRootWidget();
    void connectToNode(quint16 node_id, quint16 layer_id, double range);

    void setConnectedToNode(bool connected);
    void setNodeID(quint16 node_id);
    void setRange(double range);
    void setNodeLayer(quint16 layer_id);
    void setWidgetImage(const QString &imgName, ImageType imgType, double scale);

    bool isRootWidget() const;
    virtual DragWidget::DragWidgetType getWidgetType() const;
    QString getImageName(ImageType imgType) const;
    bool isConnectedToNode() const;
    quint16 getNodeID() const;
    quint16 getLayerID() const;
    double getNodeRange() const;
    QPoint getPosition() const;
    QSize getWidgetSize() const;
    double getImgScale() const;
protected:
    virtual void processReceivedData(const DataFrame &data);
    virtual void processDataSend(const DataFrame &data);
public slots:
    void onNodeReceivedData(const DataFrame &data);
    void onNodeSendData(const DataFrame &data);
    void onStateUpdate();
signals:
    void sendWidgetReceivedData(const QByteArray &data, quint16 node_id, quint16 layer_id) const;
    void sendWidgetSendData(const QByteArray &data, quint16 node_id, quint16 layer_id) const;
protected:
    QString m_defaultImgName;
    QString m_rxDataImgName;
    QString m_txDataImgName;
    bool m_rootWidget;
    bool m_connectedToNode;
    QSize m_widgetSize;
    quint16 m_node_id;
    quint16 m_layer_id;
    double m_range;
    double m_imgScale;
    QTimer *m_rxStateTimer;
    QTimer *m_txStateTimer;
    QTimer *m_stateResetTimer;
    QVector<bool> m_events;
};

#endif // DRAGWIDGET_H
