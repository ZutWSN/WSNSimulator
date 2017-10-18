#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QWidget>
#include <QLabel>

class DragWidget : public QLabel
{
public:
    DragWidget(const QString imgFile, QWidget *parent = 0);
    virtual ~DragWidget();
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // DRAGWIDGET_H
