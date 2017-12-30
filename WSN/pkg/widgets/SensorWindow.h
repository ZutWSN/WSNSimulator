#ifndef SENSORWINDOW_H
#define SENSORWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "DragWidget.h"
#include "SensorNetwork.h"
#include <QPushButton>
#include <QPlainTextEdit>
#include <QComboBox>

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

    SensorWindow(QWidget *parent = 0, const QSize &windowSize = QSize(1350, 825));
    virtual ~SensorWindow();
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *e);
public slots:
    void onPressedShowNetworkState();
    void onPressedSendMsg();
    void onPressedRemoveNode();
    void onPressedSensorBroadcast();
    void onPressedShowSinkPath();
    void onWidgetReceivedData(const DataFrame &data, quint16 node_id, quint16 layer_id);
signals:
    void addNewSink(const QPoint &position, quint16 range, QWidget *uiWidget);
    void addNewCluster(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
    void addNewSensor(quint16 cluster_id, quint16 layer_id, const QPoint &position, quint16 range, QWidget *uiWidget);
    void removeSink();
    void removeNode(quint16 node_id, quint16 layer_id);
    void moveNode(quint16 node_id, quint16 layer_id, QPoint position);
private:
    void initializeUiWidgets();
    void redrawConnections();
    void drawConnection();
    void setNodeInfo(quint16 node_id, quint16 layer_id, quint16 range, bool isSink);
    bool getNodeIDAndRange(quint16 &node_id, double &node_range) const;
    bool getNodeID(quint16 &node_id) const;
    bool getNodeRange(double &node_range) const;
protected:
    QVector<DragWidget*> m_dragWidgets;
    QScopedPointer<SensorNetwork> m_sensorNetwork;
    QVector<LineConnection> m_networkConnectionLines;
    QPushButton *m_btnShowNetworkState;
    QPushButton *m_btnSendMsg;
    QPushButton *m_btnSensorBroadcast;
    QPushButton *m_btnRemoveNode;
    QPushButton *m_btnShowSinkPath;
    QPlainTextEdit *m_etxSensorMsg;
    QPlainTextEdit *m_etxLogWindow;
    QPlainTextEdit *m_etxInputNodeID;
    QPlainTextEdit *m_etxInputNodeRange;
    QPlainTextEdit *m_etxNodeIDInfo;
    QPlainTextEdit *m_etxNodeRangeInfo;
    QPlainTextEdit *m_etxNodeStateInfo;
    QPlainTextEdit *m_etxLastRxMsg;
    QComboBox *m_cbSensorList;
    bool m_sinkCreated;
    bool m_draw;
    quint16 m_id;

};

#endif // SENSORWINDOW_H
