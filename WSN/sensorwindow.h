#ifndef SENSORWINDOW_H
#define SENSORWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>

class SensorWindow : public QMainWindow
{
public:
    SensorWindow(QWidget *parent = 0);
    virtual ~SensorWindow();
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    QDockWidget *m_dockWidget;
    QLabel *m_label;
};

#endif // SENSORWINDOW_H
