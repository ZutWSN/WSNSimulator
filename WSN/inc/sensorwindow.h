#ifndef SENSORWINDOW_H
#define SENSORWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "sensorwidget.h"
#include "clusterwidget.h"

class SensorWindow : public QMainWindow
{
    Q_OBJECT
public:
    SensorWindow(QWidget *parent = 0, const QSize &windowSize = QSize(800, 800));
    virtual ~SensorWindow();
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
public slots:
    void pressedButton();
private:
    void initializeUiWidgets();
private:
    QVector<DragWidget*> m_dragWidgets;
    QWidget *m_pressButton;
};

#endif // SENSORWINDOW_H
