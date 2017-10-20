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

    DragWidget(QWidget *parent = 0, bool rootWidget = false);
    virtual ~DragWidget();
    void setWidgetImage(const QString &imgName);
    void setAsRootWidget();

    bool isRootWidget() const;
    QString getImageName() const;
private:
    QString m_imgName;
    bool m_rootWidget;
};

#endif // DRAGWIDGET_H
