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

    bool isRootWidget() const;
    virtual DragWidget::DragWidgetType getWidgetType() const;
    QString getImageName() const;
public slots:
    void onNodeReceivedData(const DataFrame &data);
private:
    QString m_imgName;
    bool m_rootWidget;
};

#endif // DRAGWIDGET_H
