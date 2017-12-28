#ifndef SENSORWINDOW_H
#define SENSORWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "DragWidget.h"
#include "SensorNetwork.h"
#include <QPushButton>
#include <QPlainTextEdit>

class SensorWindow : public QMainWindow
{
    Q_OBJECT
public:
    const QColor DIRECT_CLUSTER_COLOR = QColor(0, 0, 0);
    const QColor MAPPED_CLUSTER_COLOR = QColor(0, 0, 255);
    const QColor SENSOR_COLOR = QColor(255, 0, 255);
    const QColor MESSAGE_SENT_COLOR = QColor(255, 0, 0);
    const QColor MESSAGE_RECEIVED_COLOR = QColor(0, 255, 0);
    struct LineConnection
    {
        QLine line;
        QColor lineColor;
    };

    SensorWindow(QWidget *parent = 0, const QSize &windowSize = QSize(1000, 1000));
    virtual ~SensorWindow();
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *e);
public slots:
    void pressedButton();
    void onWidgetReceivedData(const DataFrame &data, quint16 node_id, quint16 layer_id);
signals:
    void addNewSink(const QPoint &position, quint16 range, QWidget *uiWidget);
    void addNewCluster(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
    void addNewSensor(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
    void removeSink();
    void removeNode(quint16 node_id, quint16 layer_id);
private:
    void initializeUiWidgets();
    void redrawConnections();
    void drawConnection();
protected:
    QVector<DragWidget*> m_dragWidgets;
    QScopedPointer<SensorNetwork> m_sensorNetwork;
    QVector<LineConnection> m_networkConnectionLines;
    QPushButton *m_pressButton;
    QPlainTextEdit *m_logWindow;
    bool m_sinkCreated;
    bool m_draw;
    quint16 m_id;

};

#endif // SENSORWINDOW_H
