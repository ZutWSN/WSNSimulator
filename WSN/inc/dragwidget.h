#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QWidget>
#include <QLabel>

class DragWidget : public QLabel
{
    Q_OBJECT
public:
    enum DragWidgetType
    {
        Sensor = 0,
        Cluster = 1
    };

    DragWidget(QWidget *parent = 0);
    virtual ~DragWidget();
    void setWidgetImage(const QString &imgName);
    QString getImageName();
private:
    QString m_imgName;
};

#endif // DRAGWIDGET_H
