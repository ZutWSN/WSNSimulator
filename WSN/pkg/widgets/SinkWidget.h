#ifndef SINKWIDGET_H
#define SINKWIDGET_H
#include "DragWidget.h"

class SinkWidget : public DragWidget
{
    Q_OBJECT
public:
    SinkWidget(QWidget *parent = 0, bool rootWidget = false);

    DragWidget::DragWidgetType getWidgetType() const;
private:
    void processReceivedData(const DataFrame &data) const;
    void processDataSend(const DataFrame &data) const;
    void getNewPathMsg(const QByteArray &serializedMsg, QByteArray &pathMsg) const;
signals:
    void sinkSendData(const QByteArray &data) const;
    void sinkReceivedData(const QByteArray &data) const;
};

#endif // SINKWIDGET_H
